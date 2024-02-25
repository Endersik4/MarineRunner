// Copyright Adam Bartela.All Rights Reserved

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
	PrimaryComponentTick.bCanEverTick = true;

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

	RaycastForHoverItems();
}

bool UTakeAndDrop::RaycastForHoverItems()
{
	FVector Start = MarinePawn->GetCamera()->GetComponentLocation();
	FVector End = Start + (UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetRootComponent()->GetForwardVector() * TakeDistance);

	FHitResult CurrentItemHit;
	bool bWasHit = GetWorld()->SweepSingleByChannel(CurrentItemHit, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeBox(CollisionBoxSize));

	HoverHitItem(bWasHit, CurrentItemHit);

	return bWasHit;
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

	TakeInterface->TakeItem(MarinePawn);
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
		if (HoverInterface == nullptr)
			return;
		
		HoverInterface->ItemHover(MarinePawn);
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
	
	HoverInterface->ItemUnHover(MarinePawn);
}