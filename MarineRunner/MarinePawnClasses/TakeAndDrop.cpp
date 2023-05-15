// Fill out your copyright notice in the Description page of Project Settings.


#include "TakeAndDrop.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
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

	SetLocationOfItem();
}

void UTakeAndDrop::Take()
{
	FHitResult HitResult;
	if (CheckIfPlayerCanTake(HitResult) == false) return;
	if (HitResult.GetActor() == nullptr) return;

	if (HitResult.GetActor()->ActorHasTag("Gun"))
	{
		Gun = Cast<AGun>(HitResult.GetActor());

		bIsInterpEnded = false;

		MarinePawn->SetCanChangeWeapon(false);
		MarinePawn->HideGunAndAddTheNewOne(Gun);

		Gun->EquipWeapon(MarinePawn);
	}
}

bool UTakeAndDrop::CheckIfPlayerCanTake(FHitResult& HitResult)
{
	FVector Start = MarinePawn->GetCamera()->GetComponentLocation();
	FVector End = Start + (MarinePawn->GetCamera()->GetForwardVector() * TakeDistance);

	bool hasHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeBox(FVector(20, 20, 20)));
	bool bIsTooManyItems = MarinePawn->GetWeaponInventoryComponent()->GetWeaponsStorageAmount() >= MarinePawn->GetWeaponInventoryComponent()->GetMaxAmount();

	if (hasHit == false || bIsInterpEnded == false || bIsTooManyItems) return false;

	return true;
}

void UTakeAndDrop::SetLocationOfItem()
{
	if (!MarinePawn || !Gun) return;

	if (bIsInterpEnded == true) return;
	float Delta = GetWorld()->GetDeltaSeconds();

	FVector BaseSkeletalMeshRelativeLocation = Gun->GetBaseSkeletalMesh()->GetRelativeLocation();
	FVector Location = FMath::InterpExpoOut(BaseSkeletalMeshRelativeLocation, Gun->GetRelativeLocationInPawn(), SpeedOfComingGun * Delta);
	Gun->SetActorRelativeLocation(Location);
	
	FRotator BaseSkeletalMeshRelativeRotation = Gun->GetBaseSkeletalMesh()->GetRelativeRotation();
	FRotator Rotation = FMath::InterpExpoOut(BaseSkeletalMeshRelativeRotation, Gun->GetRelativeRotationInPawn(), SpeedOfComingGun * Delta);
	Gun->SetActorRelativeRotation(Rotation);

	IsGunAtTheWeaponLocation();
}

void UTakeAndDrop::IsGunAtTheWeaponLocation()
{
	bIsInterpEnded = Gun->GetBaseSkeletalMesh()->GetRelativeLocation().Equals(Gun->GetRelativeLocationInPawn(), 0.2f);
	if (!bIsInterpEnded) return;

	Gun->SetGunSwayWhileMovingTimer();
	Gun->SetCanGunSwayTick(true);
	Gun->SetActorRelativeLocation(Gun->GetRelativeLocationInPawn());

	MarinePawn->SetGun(Gun);
	MarinePawn->SetCanChangeWeapon(true);
}

void UTakeAndDrop::DropItem()
{
	Gun = MarinePawn->GetGun();
	if (bIsInterpEnded == false || Gun == nullptr) return;
	
	if (Gun->GetCanDropGun() == false) return;

	Gun->DropTheGun();

	MarinePawn->GetWeaponInventoryComponent()->RemoveWeaponFromStorage(Gun);
	int32 AmountOfWeapons = MarinePawn->GetWeaponInventoryComponent()->GetWeaponsStorageAmount();
	
	ChangeToAnotherWeapon(AmountOfWeapons);
}

void UTakeAndDrop::ChangeToAnotherWeapon(int32 AmountOfWeapons)
{
	if (AmountOfWeapons > 0)
	{
		Gun = MarinePawn->GetWeaponInventoryComponent()->GetWeaponFromStorage(AmountOfWeapons, nullptr);
		MarinePawn->SetGun(Gun);
	}
	else
	{
		MarinePawn->GetHudWidget()->HideWeaponThings(true);
		Gun = nullptr;
		MarinePawn->SetGun(nullptr);
	}
}
