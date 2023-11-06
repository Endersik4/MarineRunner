// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/EnemyTurretPawn.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnemyGunComponent.h"

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
}

void AEnemyTurretPawn::PlayerWasSeen(bool bWas, AActor* ActorSeen)
{
	// first time turret saw it
	if (bRotateBones == false && bWas == true)
	{
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AEnemyTurretPawn::Shoot, TimeBetweenShoots, true);
	}

	if (bWas == false)
	{
		GetWorld()->GetTimerManager().ClearTimer(StartShootingHandle);
	}

	bRotateBones = bWas;
	FocusedActor = ActorSeen;
}

void AEnemyTurretPawn::RotateBonesTowardDetectedActor(float Delta)
{
	if (bRotateBones == false || IsValid(FocusedActor) == false)
		return;

	for (const FRotateTurretBone& RotateBone : RotateTurretBones)
	{
		FRotator FoundRot = UKismetMathLibrary::FindLookAtRotation(TurretSkeletalMesh->GetBoneLocation(RotateBone.BoneName), FocusedActor->GetActorLocation());
		FoundRot.Yaw -= GetActorRotation().Yaw;

		FoundRot.Pitch *= RotateBone.RotateInAxis.Pitch;
		FoundRot.Yaw *= RotateBone.RotateInAxis.Yaw;
		FoundRot.Roll *= RotateBone.RotateInAxis.Roll;

		PredictWhereToShoot(FocusedActor);

		if (RotateBone.bLimitedRotation == true)
		{
			LimitAngleAccordingToRange(FoundRot.Pitch, RotateBone.RotateAngleRange);
			LimitAngleAccordingToRange(FoundRot.Yaw, RotateBone.RotateAngleRange);
			LimitAngleAccordingToRange(FoundRot.Roll, RotateBone.RotateAngleRange);
		}

		BoneDirectionToFocusedActor(FoundRot, RotateBone.BoneName);
	}
}

void AEnemyTurretPawn::PredictWhereToShoot(AActor* Actor)
{
	FocusedActorLocation = Actor->GetActorLocation();
	float Distance = UKismetMathLibrary::Vector_Distance(GetActorLocation(), Actor->GetActorLocation()) / PredictWhereToShootDistanceDivider;

	FocusedActorLocation += Actor->GetActorUpVector() * PredictWhereToShootOffset_UP * Distance;

	if (Actor->GetInputAxisValue("Right") == 1.f)
	{
		FocusedActorLocation += Actor->GetActorRightVector() * PredictWhereToShootOffset_Right * Distance;
	}
	else if (Actor->GetInputAxisValue("Right") == -1.f)
	{
		FocusedActorLocation -= Actor->GetActorRightVector() * PredictWhereToShootOffset_Right * Distance;
	}
	if (Actor->GetInputAxisValue("Forward") == 1.f)
	{
		FocusedActorLocation += Actor->GetRootComponent()->GetForwardVector() * PredictWhereToShootOffset_Right * Distance;
	}
	else if (Actor->GetInputAxisValue("Forward") == -1.f)
	{
		FocusedActorLocation -= Actor->GetRootComponent()->GetForwardVector() * PredictWhereToShootOffset_Right * Distance;
	}
}

void AEnemyTurretPawn::LimitAngleAccordingToRange(double& Angle, const FFloatRange& Range)
{
	if (Angle < Range.GetLowerBoundValue())
	{
		Angle -= Angle - Range.GetLowerBoundValue();
	}
	else if (Angle > Range.GetUpperBoundValue())
	{
		Angle -= Angle - Range.GetUpperBoundValue();
	}
}

bool AEnemyTurretPawn::IsStillShooting()
{
	return GetWorld()->GetTimerManager().IsTimerActive(StartShootingHandle);
}