// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/TypesOfEnemy/EnemyTurretPawn.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Enemies/Components/EnemyGunComponent.h"

// Sets default values
AEnemyTurretPawn::AEnemyTurretPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TurretSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TurretSkeletalMesh"));
	RootComponent = TurretSkeletalMesh;

	TurretGunComponent = CreateDefaultSubobject<UEnemyGunComponent>(TEXT("TurretGunComponent"));
}

// Called when the game starts or when spawned
void AEnemyTurretPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemyTurretPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateBonesTowardDetectedActor(DeltaTime);
}

// Called to bind functionality to input
void AEnemyTurretPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyTurretPawn::Shoot()
{
	TurretGunComponent->CanShootAgain();
	TurretGunComponent->Shoot();

	if (IsValid(ShootAnimMontage))
		TurretSkeletalMesh->GetAnimInstance()->Montage_Play(ShootAnimMontage);
}

void AEnemyTurretPawn::PlayerWasSeen(bool bWas, AActor* ActorSeen)
{
	// first time turret saw it
	if (!bRotateBones && bWas)
	{
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AEnemyTurretPawn::Shoot, TimeBetweenShoots, true);
	}

	if (!bWas)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartShootingHandle);
	}

	bRotateBones = bWas;
	FocusedActor = ActorSeen;
}

void AEnemyTurretPawn::RotateBonesTowardDetectedActor(float Delta)
{
	if (!bRotateBones)
		return;

	if (!ensureMsgf(IsValid(FocusedActor), TEXT("Focused Actor is nullptr in EnemyTurretPawn->RotateBonesTowardDetectedActor")))
	{
		return;
	}

	for (const FRotateTurretBone& RotateBone : RotateTurretBones)
	{
		FRotator FoundRot = UKismetMathLibrary::FindLookAtRotation(TurretSkeletalMesh->GetBoneLocation(RotateBone.BoneName), FocusedActor->GetActorLocation());
		FoundRot.Yaw += GetActorRotation().Yaw * -1.f;
			
		FoundRot.Pitch *= RotateBone.RotateInAxis.Pitch;
		FoundRot.Yaw *= RotateBone.RotateInAxis.Yaw;
		FoundRot.Roll *= RotateBone.RotateInAxis.Roll;

		if (RotateBone.bLimitedRotation)
		{
			LimitAngleAccordingToRange(FoundRot.Pitch, RotateBone.RotateAngleRange);
			LimitAngleAccordingToRange(FoundRot.Yaw, RotateBone.RotateAngleRange);
			LimitAngleAccordingToRange(FoundRot.Roll, RotateBone.RotateAngleRange);
		}

		BoneDirectionToFocusedActor(FoundRot, RotateBone.BoneName);
	}
}

void AEnemyTurretPawn::LimitAngleAccordingToRange(double& Angle, const FFloatRange& Range)
{
	if (Angle < Range.GetLowerBoundValue())
	{
		Angle = Range.GetLowerBoundValue();
	}
	else if (Angle > Range.GetUpperBoundValue())
	{
		Angle = Range.GetUpperBoundValue();
	}
}

bool AEnemyTurretPawn::IsStillShooting()
{
	return GetWorld()->GetTimerManager().IsTimerActive(StartShootingHandle);
}