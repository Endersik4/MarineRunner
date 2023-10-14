// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeAndDrop.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Interfaces/TakeInterface.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"

// Sets default values for this component's properties
UTakeAndDrop::UTakeAndDrop()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UTakeAndDrop::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}


// Called every frame
void UTakeAndDrop::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CheckIfWeaponIsInEquipPosition();
	RaycastForHoverItems();
}

bool UTakeAndDrop::RaycastForHoverItems()
{
	if (IsValid(MarinePawn) == false)
		return false;

	FVector Start = MarinePawn->GetCamera()->GetComponentLocation();
	FVector End = Start + (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetRootComponent()->GetForwardVector() * TakeDistance);

	FHitResult CurrentItemHit;
	bool bWasHit = GetWorld()->SweepSingleByChannel(CurrentItemHit, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeBox(CollisionBoxSize));

	HoverHitItem(bWasHit, CurrentItemHit);

	if (bWasHit == false || bWeaponIsInEquipPosition == false)
	{
		return false;
	}

	return true;
}

void UTakeAndDrop::Take()
{
	if (RaycastForHoverItems() == false)
	{
		return;
	}
	if (IsValid(LastHitResult.GetActor()) == false)
	{
		return;
	}

	TakeInterface = Cast<ITakeInterface>(LastHitResult.GetActor());
	if (TakeInterface == nullptr)
		return;

	TakeInterface->TakeItem(MarinePawn, bIsItWeapon);
	
	if (bIsItWeapon == false) TakeInterface = nullptr;
}

bool UTakeAndDrop::WhetherRaycastOnTheSameItem(const FHitResult& CurrentItemHit)
{
	if (IsValid(LastHitResult.GetActor()))
	{
		if (LastHitResult.GetActor() == CurrentItemHit.GetActor()) return false;
	}

	return true;
}

void UTakeAndDrop::HoverHitItem(const bool& bWasHit, const FHitResult& CurrentItemHit)
{
	if (bWasHit == true)
	{
		if (WhetherRaycastOnTheSameItem(CurrentItemHit) == false) return;

		DisableLastHoveredItem();
		LastHitResult = CurrentItemHit;

		ITakeInterface* HoverInterface = Cast<ITakeInterface>(CurrentItemHit.GetActor());
		if (HoverInterface)
		{
			HoverInterface->ItemHover(MarinePawn->GetHudWidget());
		}
	}
	else if (IsValid(LastHitResult.GetActor()) == true)
	{
		DisableLastHoveredItem();
		LastHitResult = CurrentItemHit;
	}
}

void UTakeAndDrop::DisableLastHoveredItem()
{
	if (IsValid(LastHitResult.GetActor()) == false) return;

	ITakeInterface* HoverInterface = Cast<ITakeInterface>(LastHitResult.GetActor());
	if (HoverInterface == nullptr)
		return;
	
	HoverInterface->ItemUnHover(MarinePawn->GetHudWidget());
}

void UTakeAndDrop::CheckIfWeaponIsInEquipPosition()
{
	if (IsValid(MarinePawn) == false || !TakeInterface || bIsItWeapon == false) 
		return;

	bWeaponIsInEquipPosition = TakeInterface->MoveItemToEquipPosition(SpeedOfComingGun * GetWorld()->GetDeltaSeconds());
}

void UTakeAndDrop::DropItem()
{
	if (IsValid(MarinePawn) == false || bWeaponIsInEquipPosition == false) 
		return;

	TakeInterface = Cast<ITakeInterface>(MarinePawn->GetWeaponHandlerComponent()->GetGun());
	if (TakeInterface == nullptr)
		return;

	TakeInterface = Cast<ITakeInterface>(TakeInterface->DropItem());
}