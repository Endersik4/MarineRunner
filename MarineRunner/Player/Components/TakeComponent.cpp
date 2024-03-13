// Copyright Adam Bartela.All Rights Reserved

#include "TakeComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Interfaces/TakeInterface.h"

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
	MarinePawn->OnDestroyed.AddDynamic(this, &UTakeAndDrop::OnOwnerDestroyed);
}

void UTakeAndDrop::OnOwnerDestroyed(AActor* DestroyedActor)
{
	TakeReleased();
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
	if (!GetWorld()->GetTimerManager().IsTimerActive(ConstantlyTakeHandle))
		GetWorld()->GetTimerManager().SetTimer(ConstantlyTakeHandle, this, &UTakeAndDrop::Take, TakeAnotherItemTime, true);

	if (!RaycastForHoverItems())
		return;
	if (!IsValid(LastHitResult.GetActor()))
		return;

	TakeInterface = Cast<ITakeInterface>(LastHitResult.GetActor());
	if (!TakeInterface)
		return;

	TakeInterface->TakeItem(MarinePawn);
}

void UTakeAndDrop::TakeReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(ConstantlyTakeHandle);
}

bool UTakeAndDrop::WhetherRaycastOnTheSameItem(const FHitResult& CurrentItemHit)
{
	if (IsValid(LastHitResult.GetActor()))
	{
		if (LastHitResult.GetActor() == CurrentItemHit.GetActor()) 
			return false;
	}

	return true;
}

void UTakeAndDrop::HoverHitItem(const bool& bWasHit, const FHitResult& CurrentItemHit)
{
	if (bWasHit)
	{
		if (!WhetherRaycastOnTheSameItem(CurrentItemHit)) 
			return;

		DisableLastHoveredItem();
		LastHitResult = CurrentItemHit;

		ITakeInterface* HoverInterface = Cast<ITakeInterface>(CurrentItemHit.GetActor());
		if (!HoverInterface)
			return;
		
		HoverInterface->ItemHover(MarinePawn);
	}
	else if (IsValid(LastHitResult.GetActor()))
	{
		DisableLastHoveredItem();
		LastHitResult = CurrentItemHit;
	}
}

void UTakeAndDrop::DisableLastHoveredItem()
{
	if (!IsValid(LastHitResult.GetActor())) 
		return;

	ITakeInterface* HoverInterface = Cast<ITakeInterface>(LastHitResult.GetActor());
	if (!HoverInterface)
		return;
	
	HoverInterface->ItemUnHover(MarinePawn);
}