// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/AlbertosClasses/Components/CraftItemAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"

// Sets default values for this component's properties
UCraftItemAlbertosComponent::UCraftItemAlbertosComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCraftItemAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	AlbertosPawn = Cast<AAlbertosPawn>(GetOwner());
	
}


// Called every frame
void UCraftItemAlbertosComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveCraftedItemToFinalPosition(DeltaTime);

}

#pragma region //////////////// Crafting ////////////////
void UCraftItemAlbertosComponent::CraftPressed(class AMarineCharacter* Player, FString ItemRowNameToSpawn, FTimerHandle* CraftTimeHandle)
{
	SpawnedItem = GetWorld()->SpawnActor<APickupItem>(RecipesOfCraftableItems[ChoiceOfCraftableItem].ItemObject, SpawnLocation, SpawnRotation);
	if (SpawnedItem == nullptr) return;

	APickupItem* SpawnedItem;
	FVector SpawnLocation = AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(FName(TEXT("ItemSpawnLocation")));
	SpawnLocation += AlbertosPawn->GetActorForwardVector() * RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftLocation.X;
	SpawnLocation += AlbertosPawn->GetActorRightVector() * RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftLocation.Y;

	FRotator SpawnRotation = AlbertosPawn->GetActorRotation() + RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftRotation;


	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel1, ECR_Ignore);
	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel3, ECR_Ignore);

	CraftedItem = SpawnedCraftingItem;
	FItemStruct* ItemData = Player->GetInventoryComponent()->GetItemInformationFromDataTable(CraftedItem->GetItemRowName());
	if (ItemData == nullptr)
		return;

	//SpawnedItem->SetItemAmount(SpawnedItem->GetItemSettings().Item_Amount * CraftingMultiplier);
	MarinePawn->UpdateAlbertosInventory();


	if (ItemData->Item_CraftScale != FVector(0.f))
	{
		bShouldScaleCraftedItem = true;
		TargetScaleOfCraftedItem = ItemData->Item_CraftScale;
	}

	AlbertosPawn->ToggleDoor();
	AlbertosPawn->EnableCraftingAnimation(AlbertosPawn->GetAlbertosSkeletal());

	CraftedItem->SetDissolveMaterial(Player, OverlayCraftingMaterial);

	if (CraftingItemSound) 
		SpawnedCraftingSound = UGameplayStatics::SpawnSoundAttached(CraftingItemSound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);

	CraftingTimeHandle = CraftTimeHandle;

	const float FirstDelay = TimeOfCraftingRuntimeSound + TimeAfterStartingCraftSound;
	GetWorld()->GetTimerManager().SetTimer(ShouldLoopCraftingSoundHandle, this, &UCraftItemAlbertosComponent::ShouldLoopCraftingSound, TimeOfCraftingRuntimeSound, true, FirstDelay);
}

void UCraftItemAlbertosComponent::ShouldLoopCraftingSound()
{
	if (SpawnedCraftingSound == nullptr || CraftingTimeHandle == nullptr) return;

	if (GetWorld()->GetTimerManager().GetTimerRemaining(*CraftingTimeHandle) > TimeLeftEndCraftingLoop)
	{
		SpawnedCraftingSound->Play(TimeAfterStartingCraftSound);
	}
	else GetWorld()->GetTimerManager().ClearTimer(ShouldLoopCraftingSoundHandle);
}

void UCraftItemAlbertosComponent::CraftingFinished()
{
	EnableCraftingAnimation(AlbertosSkeletalMesh, false, 0.f);

	if (CraftedItem == nullptr) return;

	CraftedItem->ChangeSimulatingPhysics(false);

	bMoveCraftedItemToFinalPosition = true;
	FinalLocation = CraftedItem->GetActorLocation() + GetActorForwardVector() * FVector::Distance(AlbertosSkeletalMesh->GetSocketLocation(FName(TEXT("FinalItemPosition"))), CraftedItem->GetActorLocation());
}
#pragma endregion

#pragma region /////////////////////// Crafted Item ///////////////////////////
void UCraftItemAlbertosComponent::MoveCraftedItemToFinalPosition(float Delta)
{
	if (bMoveCraftedItemToFinalPosition == false || CraftedItem == nullptr) return;

	if (!CraftedItem->GetActorLocation().Equals(FinalLocation, 10.f))
	{
		FVector NewLocation = FMath::VInterpTo(CraftedItem->GetActorLocation(), FinalLocation, Delta, ItemMoveSpeedAfterCrafting);
		CraftedItem->SetActorLocation(NewLocation);

		if (bShouldScaleCraftedItem == false) return;

		FVector NewScale = FMath::VInterpTo(CraftedItem->GetActorScale3D(), TargetScaleOfCraftedItem, Delta, ItemMoveSpeedAfterCrafting * 2.2f);
		CraftedItem->SetActorScale3D(NewScale);
	}
	else
	{
		if (bShouldScaleCraftedItem == true) CraftedItem->SetActorScale3D(TargetScaleOfCraftedItem);

		bMoveCraftedItemToFinalPosition = false;
		bShouldScaleCraftedItem = false;

		CraftedItem->ChangeSimulatingPhysics(true);

		CraftedItem->SetCollisionNewResponse(ECC_GameTraceChannel1, ECR_Block);
		CraftedItem->SetCollisionNewResponse(ECC_GameTraceChannel3, ECR_Block);
		CraftedItem = nullptr;

		if (bPlayerWasClose == true) CheckIfThePlayerIsNear();
	}
}
#pragma endregion