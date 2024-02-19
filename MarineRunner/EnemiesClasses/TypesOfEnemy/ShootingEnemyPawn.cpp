// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/TypesOfEnemy/ShootingEnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyGunComponent.h"
#include "MarineRunner/EnemiesClasses/EnemyAiController.h"

AShootingEnemyPawn::AShootingEnemyPawn()
{
	EnemyGunComponent = CreateDefaultSubobject<UEnemyGunComponent>(TEXT("Enemy Gun Component"));
}

void AShootingEnemyPawn::Tick(float DeltaTime)
{
	if (bIsDead == true)
		return;

	Super::Tick(DeltaTime);

	if (bEnemyDetectedTarget == true)
		FocusBonesOnPlayerWhenPlayerDetected();
}

void AShootingEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	Super::ApplyDamage(NewDamage,NewImpulseForce, NewHit, BulletActor, NewSphereRadius);

	KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);
}

bool AShootingEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	bool bKilled = Super::KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);

	if (bKilled == true)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
		SetEnemyKilledInAIController();
	}
	else
	{
		EnemyRunAway();
	}
	return bKilled;
}

bool AShootingEnemyPawn::EnemyRunAway()
{
	if (Health > MaxEnemyHealthForRunAway || bCanEnemyRunAway == false)
		return false;

	float ShouldRunAwayRandom = FMath::FRandRange(0.f, 100.f);
	if (ShouldRunAwayRandom <= ChanceOfEnemyToRunAway)
	{
		ShouldRunAway();
		return true;
	}

	return false;
}

#pragma region ////////////// ENEMY SEE PLAYER //////////////
void AShootingEnemyPawn::SawTheTarget(bool bSaw, AActor* SeenTarget, bool bStartAttackingTheTarget)
{
	FocusedActor = SeenTarget;
	bEnemyDetectedTarget = bSaw;

	PlayPrepareToShootAnimation(bSaw);

	if (bStartAttackingTheTarget == false)
		return;

	EnemyIndicatorWidgetComponent->SetVisibility(bSaw);

	const float& StartShootingTime = TimeToStartShooting + FMath::FRandRange(StartShootingRandomTimeRange.GetLowerBoundValue(), StartShootingRandomTimeRange.GetUpperBoundValue());
	if (bSaw == true)
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AShootingEnemyPawn::StartShooting, StartShootingTime, false);
	else
		GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
}

void AShootingEnemyPawn::StartShooting()
{
	if (bIsDead == true)
		return;

	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &AShootingEnemyPawn::Shoot, ShootTime, true, 0.f);
}

void AShootingEnemyPawn::Shoot()
{
	if (EnemyGunComponent->CanShootAgain() == false)
		return;

	EnemyGunComponent->Shoot();
	PlayShootMontageAnimation();
}

FRotator AShootingEnemyPawn::FocusBoneOnPlayer(FName BoneName, bool bLookStraight)
{
	FRotator BoneRotation = FRotator(0.f);
	FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneName),
		EnemyGunComponent->PredictWhereToShoot(bLookStraight));
	BoneRotation.Roll = FoundRotation.Pitch * -1.f;
	BoneRotation.Yaw = FoundRotation.Yaw - GetActorRotation().Yaw;

	return BoneRotation;
}

void AShootingEnemyPawn::ShouldRunAway()
{
	SetEnemyKilledInAIController();

	SawTheTarget(false);
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
	GetWorld()->GetTimerManager().ClearTimer(StartShootingHandle);

	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
}
#pragma endregion

void AShootingEnemyPawn::SetEnemyKilledInAIController()
{
	AEnemyAiController* EnemyAIController = Cast<AEnemyAiController>(GetController());
	if (IsValid(EnemyAIController) == false)
		return;

	EnemyAIController->EnemyKilled(true);
}