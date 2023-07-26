// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeAndDrop.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

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

	SetLocationOfItem();
	CheckIfPlayerCanTake();
}

void UTakeAndDrop::Take()
{
	if (CheckIfPlayerCanTake() == false) return;
	if (HitResult.GetActor() == nullptr) return;

	TakeInterface = Cast<ITakeInterface>(HitResult.GetActor());
	if (TakeInterface)
	{
		TakeInterface->TakeItem(MarinePawn, bIsItWeapon);
		MarinePawn->GetHudWidget()->HideItemHover(ESlateVisibility::Hidden);
		if (bIsItWeapon == false) TakeInterface = nullptr;
	}
}

bool UTakeAndDrop::CheckIfPlayerCanTake()
{
	FVector Start = MarinePawn->GetCamera()->GetComponentLocation();
	FVector End = Start + (UGameplayStatics::GetPlayerController(GetWorld(),0)->GetRootComponent()->GetForwardVector() * TakeDistance);
	
	FHitResult FirstHit;
	bool hasHit = GetWorld()->SweepSingleByChannel(FirstHit, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeBox(FVector(20, 20, 20)));

	if (hasHit == true)
	{
		HitResult = FirstHit;
		ITakeInterface* HoverInterface = Cast<ITakeInterface>(HitResult.GetActor());
		if (HoverInterface)
		{
			HoverInterface->ItemHover(MarinePawn->GetHudWidget());
		}
	}
	else if (HitResult.GetActor())
	{
		MarinePawn->GetHudWidget()->HideItemHover(ESlateVisibility::Hidden);
		HitResult = FirstHit;
	}

	if (hasHit == false || bIsInterpEnded == false) return false;

	return true;
}

void UTakeAndDrop::SetLocationOfItem()
{
	if (!MarinePawn || !TakeInterface || bIsItWeapon == false) return;

	bIsInterpEnded = TakeInterface->ItemLocationWhenGrabbed(SpeedOfComingGun * GetWorld()->GetDeltaSeconds());
}

void UTakeAndDrop::DropItem()
{
	if (!MarinePawn || bIsInterpEnded == false) return;
	TakeInterface = Cast<ITakeInterface>(MarinePawn->GetGun());

	if (TakeInterface)
	{
		TakeInterface = Cast<ITakeInterface>(TakeInterface->DropItem());
	}
}