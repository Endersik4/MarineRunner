// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Components/CraftItemAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Objects/Interactive/PickableItem.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"

// Sets default values for this component's properties
UCraftItemAlbertosComponent::UCraftItemAlbertosComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCraftItemAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Albertos Pawn is nullptr in CraftItemAlbertosComponent!")))
	{
		AlbertosPawn = Cast<AAlbertosPawn>(GetOwner());
	}
}

void UCraftItemAlbertosComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveCraftedItemToFinalPosition(DeltaTime);
}

#pragma region //////////////// craft item ////////////////
void UCraftItemAlbertosComponent::CraftPressed(TObjectPtr<class AMarineCharacter> Player, const FItemStruct* ItemToCraft, int32 ItemAmountMultiplier)
{
	if (!IsValid(AlbertosPawn))
		return;

	if (!ItemToCraft || !IsValid(Player))
		return;

	CraftedItem = SpawnCraftedItem(ItemToCraft);
	if (!IsValid(CraftedItem))
		return;

	CraftedItem->SetItemAmountMultiplier(ItemAmountMultiplier);

	if (ItemToCraft->InitialCraftScale != FVector(1.f))
	{
		bShouldScaleCraftedItem = true;
		TargetScaleOfCraftedItem = ItemToCraft->TargetScaleAfterCrafting;
	}

	AlbertosPawn->CallToggleOpenDoor(true);
	AlbertosPawn->EnableCraftingAnimation(true);

	CraftedItem->SetDissolveMaterial(Player, ItemToCraft->Item_TimeCraft, OverlayCraftingMaterial);

	StartPlayingCraftSound(ItemToCraft->Item_TimeCraft);
}

TObjectPtr<APickupItem> UCraftItemAlbertosComponent::SpawnCraftedItem(const FItemStruct* ItemToCraft)
{
	if (!ItemToCraft->ItemObject.TryLoadClass<UObject>())
		return nullptr;

	const FTransform& ItemToCraftTransform = ItemToCraftOffsetTransform(ItemToCraft->InitialCraftLocationOffset, ItemToCraft->InitialCraftRotation, ItemToCraft->InitialCraftScale);

	TObjectPtr<APickupItem> SpawnedItem = GetWorld()->SpawnActor<APickupItem>(ItemToCraft->ItemObject.TryLoadClass<UObject>(), ItemToCraftTransform);
	if (!IsValid(SpawnedItem))
		return nullptr;

	SpawnedItem->SaveItemIfSpawnedRunTime();
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

	const FRotator& SpawnRotation = AlbertosPawn->GetActorRotation() + OffsetRotator;

	return FTransform(SpawnRotation, SpawnLocation, ItemInitialScale);
}

void UCraftItemAlbertosComponent::CraftingFinished()
{
	if (!IsValid(AlbertosPawn))
		return;

	AlbertosPawn->EnableCraftingAnimation(false);
	if (!IsValid(AlbertosPawn->GetAlbertosSkeletal()))
		return;
	
	if (!IsValid(CraftedItem))
		return;
	if (!IsValid(CraftedItem->GetItemMesh()))
		return;

	CraftedItem->GetItemMesh()->SetSimulatePhysics(false);

	bMoveCraftedItemToFinalPosition = true;
	FinalCraftedItemLocation = CraftedItem->GetActorLocation() + AlbertosPawn->GetActorForwardVector() * FVector::Distance(AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(FinalItemLocationSocketName), CraftedItem->GetActorLocation());
}
#pragma endregion

#pragma region ///////// crafting sounds //////////////

void UCraftItemAlbertosComponent::StartPlayingCraftSound(const float& TimeToCraftAnItem)
{
	if (!IsValid(AlbertosPawn))
		return;
	if (!IsValid(AlbertosPawn->GetAlbertosSkeletal()))
		return;

	MiddleCraftSoundCounter = CalculateHowManyMiddleCraftSoundHaveToPlay(TimeToCraftAnItem);

	if (IsValid(Craft_Start_Sound))
		UGameplayStatics::SpawnSoundAttached(Craft_Start_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
	else
		UE_LOG(LogTemp, Warning, TEXT("Craft Start Sound is nullptr in CraftItemAlbertosComponent!"));

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
	if (!IsValid(AlbertosPawn))
		return;
	if (!IsValid(AlbertosPawn->GetAlbertosSkeletal()))
		return;

	if (MiddleCraftSoundCounter <= 0)
	{
		if (IsValid(Craft_End_Sound))
			UGameplayStatics::SpawnSoundAttached(Craft_End_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
		else
			UE_LOG(LogTemp, Warning, TEXT("Craft End Sound is nullptr in CraftItemAlbertosComponent!"));

		return;
	}

	if (IsValid(Craft_End_Sound))
		UGameplayStatics::SpawnSoundAttached(Craft_Middle_Sound, AlbertosPawn->GetAlbertosSkeletal(), ItemSpawnLocationSocketName);
	else
		UE_LOG(LogTemp, Warning, TEXT("Craft Middle Sound is nullptr in CraftItemAlbertosComponent!"));

	MiddleCraftSoundCounter--;

	FTimerHandle CraftMiddleHandle;
	GetWorld()->GetTimerManager().SetTimer(CraftMiddleHandle, this, &UCraftItemAlbertosComponent::PlayMiddleCraftSound, Craft_Middle_Sound->GetDuration(), false);
}
#pragma endregion

#pragma region /////////////////////// Move Item to Final Position ///////////////////////////
void UCraftItemAlbertosComponent::MoveCraftedItemToFinalPosition(float Delta)
{
	if (!bMoveCraftedItemToFinalPosition || !IsValid(CraftedItem)) 
		return;

	if (!CraftedItem->GetActorLocation().Equals(FinalCraftedItemLocation, FinalLocationItemTolerance))
	{
		const FVector& NewItemLocation = FMath::VInterpTo(CraftedItem->GetActorLocation(), FinalCraftedItemLocation, Delta, ItemMoveSpeedAfterCrafting);
		CraftedItem->SetActorLocation(NewItemLocation);

		if (!bShouldScaleCraftedItem) 
			return;

		const FVector& NewItemScale = FMath::VInterpTo(CraftedItem->GetActorScale3D(), TargetScaleOfCraftedItem, Delta, ItemScaleSpeedAfterCrafting);
		CraftedItem->SetActorScale3D(NewItemScale);

		return;
	}

	ItemWasMoved();
}

void UCraftItemAlbertosComponent::ItemWasMoved()
{
	if (!IsValid(CraftedItem))
		return;
	if (!IsValid(CraftedItem->GetItemMesh()))
		return;

	if (bShouldScaleCraftedItem)
		CraftedItem->SetActorScale3D(TargetScaleOfCraftedItem);

	bMoveCraftedItemToFinalPosition = false;
	bShouldScaleCraftedItem = false;

	CraftedItem->GetItemMesh()->SetSimulatePhysics(true);
	CraftedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block); // Take Trace
	CraftedItem->GetItemMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel3, ECR_Block); // Bullet Trace

	CraftedItem = nullptr;
}
#pragma endregion

const bool UCraftItemAlbertosComponent::IsCraftedItemValid() const
{
	return IsValid(CraftedItem);
}