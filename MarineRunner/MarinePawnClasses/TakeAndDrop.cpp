// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeAndDrop.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"

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

	SetLocationOfItem(DeltaTime);
}

void UTakeAndDrop::Take()
{
	if (MarinePawn == nullptr) return;
	
	FVector Start = MarinePawn->GetCamera()->GetComponentLocation();
	FVector End = Start + (MarinePawn->GetCamera()->GetForwardVector() * TakeDistance);
	FHitResult HitResult;

	bool hasHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeBox(FVector(20, 20, 20)));

	//if (hasHit == false || bHaveItem == true) return;
	if (hasHit == false) return;

	if (HitResult.GetActor()->ActorHasTag("Gun"))
	{
		
		Gun = Cast<AGun>(HitResult.GetActor());
		if (Gun)
		{
			bHaveItem = true;
			bIsLerpEnded = false;
			Gun->EquipWeapon(MarinePawn);
		}
	}
}

void UTakeAndDrop::SetLocationOfItem(float Delta)
{
	if (!MarinePawn || !Gun) return;

	if (bHaveItem == false || bIsLerpEnded == true) return;

	FVector BaseSkeletalMeshRelativeLocation = Gun->GetBaseSkeletalMesh()->GetRelativeLocation();
	FVector Location = FMath::InterpExpoOut(BaseSkeletalMeshRelativeLocation, Gun->GetRelativeLocationInPawn(), SpeedOfComingGun * Delta);
	Gun->SetActorRelativeLocation(Location);

	FRotator BaseSkeletalMeshRelativeRotation = Gun->GetBaseSkeletalMesh()->GetRelativeRotation();
	FRotator Rotation = FMath::InterpExpoOut(BaseSkeletalMeshRelativeRotation, Gun->GetRelativeRotationInPawn(), SpeedOfComingGun * Delta);
	Gun->SetActorRelativeRotation(Rotation);

	bIsLerpEnded = Gun->GetBaseSkeletalMesh()->GetRelativeLocation().Equals(Gun->GetRelativeLocationInPawn(), 0.2f);
	if (bIsLerpEnded && MarinePawn)
	{
		Gun->SetGunSwayWhileMovingTimer();
		Gun->SetActorRelativeLocation(Gun->GetRelativeLocationInPawn());
		Gun->SetCanGunSwayTick(true);

		MarinePawn->SetHasWeapon(true);
		MarinePawn->SetGun(Gun);
	}
}

void UTakeAndDrop::DropItem()
{
	if (!MarinePawn || !Gun || bHaveItem == false) return;

	if (Gun->GetCanDropGun() == false) return;
	Gun->UnequipWeapon();
	bHaveItem = false;

	MarinePawn->SetHasWeapon(false);
	MarinePawn->SetGun(nullptr);
	MarinePawn->RemoveEquipedGun(Gun);

	Gun = nullptr;
}
