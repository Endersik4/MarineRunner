// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/AlbertosClasses/Components/CraftItemAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"

// Sets default values for this component's properties
UCraftItemAlbertosComponent::UCraftItemAlbertosComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCraftItemAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	AlbertosPawn = Cast<AAlbertosPawn>(GetOwner());
}

void UCraftItemAlbertosComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveCraftedItemToFinalPosition(DeltaTime);
}

#pragma region //////////////// craft item ////////////////
void UCraftItemAlbertosComponent::CraftPressed(class AMarineCharacter* Player, const FItemStruct* ItemToCraft, int32 ItemAmountMultiplier)
{
	if (ItemToCraft == nullptr)
		return;

	CraftedItem = SpawnCraftedItem(ItemToCraft);
	if (IsValid(CraftedItem) == false)
		return;

	CraftedItem->SetItemAmountMultiplier(ItemAmountMultiplier);

	if (ItemToCraft->InitialCraftScale != FVector(1.f))
	{
		bShouldScaleCraftedItem = true;
		TargetScaleOfCraftedItem = ItemToCraft->TargetScaleAfterCrafting;
	}

	AlbertosPawn->CallToggleOpenDoor(true);
	AlbertosPawn->EnableCraftingAnimation(AlbertosPawn->GetAlbertosSkeletal());

	CraftedItem->SetDissolveMaterial(Player, ItemToCraft->Item_TimeCraft, OverlayCraftingMaterial);

	StartPlayingCraftSound(ItemToCraft->Item_TimeCraft);

}

APickupItem* UCraftItemAlbertosComponent::SpawnCraftedItem(const FItemStruct* ItemToCraft)
{
	FTransform ItemToCraftTransform = ItemToCraftOffsetTransform(ItemToCraft->InitialCraftLocationOffset, ItemToCraft->InitialCraftRotation, ItemToCraft->InitialCraftScale);

	APickupItem* SpawnedItem = GetWorld()->SpawnActor<APickupItem>(ItemToCraft->ItemObject, ItemToCraftTransform);
	if (IsValid(SpawnedItem) == false)
		return nullptr;

	SpawnedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore); // Take Trace
	SpawnedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Ignore); // Bullet Trace

	return SpawnedItem;
}

FTransform UCraftItemAlbertosComponent::ItemToCraftOffsetTransform(const FVector& OffsetVector, const FRotator& OffsetRotator, const FVector& ItemInitialScale)
{
	FVector SpawnLocation = AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(ItemSpawnLocationSocketName);
	SpawnLocation += AlbertosPawn->GetActorForwardVector() * OffsetVector.X;
	SpawnLocation += AlbertosPawn->GetActorRightVector() * OffsetVector.Y;
	SpawnLocation += AlbertosPawn->GetActorUpVector() * OffsetVector.Z;

	FRotator SpawnRotation = AlbertosPawn->GetActorRotation() + OffsetRotator;

	return FTransform(SpawnRotation, SpawnLocation, ItemInitialScale);
}

void UCraftItemAlbertosComponent::CraftingFinished()
{
	AlbertosPawn->EnableCraftingAnimation(AlbertosPawn->GetAlbertosSkeletal(), false);

	if (IsValid(CraftedItem) == false)
		return;

	CraftedItem->GetItemMesh()->SetSimulatePhysics(false);

	bMoveCraftedItemToFinalPosition = true;
	FinalLocationItem = CraftedItem->GetActorLocation() + AlbertosPawn->GetActorForwardVector() * FVector::Distance(AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(FName(TEXT("FinalItemPosition"))), CraftedItem->GetActorLocation());
}
#pragma endregion

#pragma region ///////// crafting sounds //////////////

void UCraftItemAlbertosComponent::StartPlayingCraftSound(const float & TimeToCraftAnItem)
{
	MiddleCraftSoundCounter = CalculateHowManyMiddleCraftSoundHaveToPlay(TimeToCraftAnItem);

	UGameplayStatics::SpawnSoundAttached(Craft_Start_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
	FTimerHandle CraftStartHandle;
	GetWorld()->GetTimerManager().SetTimer(CraftStartHandle, this, &UCraftItemAlbertosComponent::PlayMiddleCraftSound, Craft_Start_Sound->GetDuration(), false);
}

int32 UCraftItemAlbertosComponent::CalculateHowManyMiddleCraftSoundHaveToPlay(const float& TimeToCraftAnItem)
{
	if (TimeToCraftAnItem <= (Craft_Start_Sound->GetDuration() + Craft_End_Sound->GetDuration()))
		return 0;

	return (int)(TimeToCraftAnItem - Craft_Start_Sound->GetDuration() - Craft_End_Sound->GetDuration()) / Craft_Middle_Sound->GetDuration();
}

void UCraftItemAlbertosComponent::PlayMiddleCraftSound()
{
	if (MiddleCraftSoundCounter <= 0)
	{
		UGameplayStatics::SpawnSoundAttached(Craft_End_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
		return;
	}

	UGameplayStatics::SpawnSoundAttached(Craft_Middle_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
	MiddleCraftSoundCounter--;

	FTimerHandle CraftMiddleHandle;
	GetWorld()->GetTimerManager().SetTimer(CraftMiddleHandle, this, &UCraftItemAlbertosComponent::PlayMiddleCraftSound, Craft_Middle_Sound->GetDuration(), false);
}
#pragma endregion

#pragma region /////////////////////// Move Item to Final Position ///////////////////////////
void UCraftItemAlbertosComponent::MoveCraftedItemToFinalPosition(float Delta)
{
	if (bMoveCraftedItemToFinalPosition == false || IsValid(CraftedItem) == false) 
		return;

	if (CraftedItem->GetActorLocation().Equals(FinalLocationItem, 10.f) == false)
	{
		FVector NewLocation = FMath::VInterpTo(CraftedItem->GetActorLocation(), FinalLocationItem, Delta, ItemMoveSpeedAfterCrafting);
		CraftedItem->SetActorLocation(NewLocation);

		if (bShouldScaleCraftedItem == false) 
			return;

		FVector NewScale = FMath::VInterpTo(CraftedItem->GetActorScale3D(), TargetScaleOfCraftedItem, Delta, ItemScaleSpeedAfterCrafting);
		CraftedItem->SetActorScale3D(NewScale);

		return;
	}

	ItemWasMoved();
}

void UCraftItemAlbertosComponent::ItemWasMoved()
{
	if (bShouldScaleCraftedItem == true)
		CraftedItem->SetActorScale3D(TargetScaleOfCraftedItem);

	bMoveCraftedItemToFinalPosition = false;
	bShouldScaleCraftedItem = false;

	CraftedItem->GetItemMesh()->SetSimulatePhysics(true);
	CraftedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // Take Trace
	CraftedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // Bullet Trace

	CraftedItem = nullptr;
}
#pragma endregion

const bool UCraftItemAlbertosComponent::isCraftedItemValid() const
{
	return IsValid(CraftedItem);
}