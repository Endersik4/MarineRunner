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

void AShootingEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	SetUpShootAlert();
}

void AShootingEnemyPawn::Tick(float DeltaTime)
{
	if (bIsDead == true)
		return;

	Super::Tick(DeltaTime);

	if (bEnemyDetectedTarget == true)
		FocusBonesOnPlayerWhenPlayerDetected();

	ChangeParameterInAlertMaterial(DeltaTime);
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
		bStartAlert = false;
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

	float RandomPercent = FMath::FRandRange(0.f, 100.f);
	if (PercentForEnemyRunaway > RandomPercent)
	{
		ShouldRunAway();
		return true;
	}

	return false;
}

void AShootingEnemyPawn::ShouldRunAway()
{
	SetEnemyKilledInAIController();

	if (bEnemyDetectedTarget == true)
		PlayPrepareToShootAnimation(false);

	SawTheTarget(false);
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
	GetWorld()->GetTimerManager().ClearTimer(StartShootingHandle);
	ResetAlertMaterial();

	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
}
#pragma region ////////////// ENEMY SEE PLAYER //////////////
void AShootingEnemyPawn::SawTheTarget(bool bSaw, AActor* SeenTarget, bool bStartAttackingTheTarget)
{
	if (bSaw == bEnemyDetectedTarget)
		return;

	FocusedActor = SeenTarget;

	PlayPrepareToShootAnimation(bSaw);
	EnemyIndicatorWidgetComponent->SetVisibility(bSaw);

	if (bStartAttackingTheTarget == false)
		return;

	bEnemyDetectedTarget = bSaw;


	const float& StartShootingTime = TimeToStartShooting + FMath::FRandRange(StartShootingRandomTimeRange.GetLowerBoundValue(), StartShootingRandomTimeRange.GetUpperBoundValue());
	if (bSaw == true)
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AShootingEnemyPawn::StartShooting, StartShootingTime, false);
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
		bStartAlert = false;
	}
}

void AShootingEnemyPawn::StartShooting()
{
	if (bIsDead == true)
		return;

	bStartAlert = true;
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &AShootingEnemyPawn::Shoot, ShootTime, true);
}

void AShootingEnemyPawn::Shoot()
{
	if (EnemyGunComponent->CanShootAgain() == false)
		return;

	ResetAlertMaterial();

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
#pragma endregion

#pragma region //////// ALERT ABOUT SHOOT /////////
void AShootingEnemyPawn::SetUpShootAlert()
{
	if (bAlertAboutShoot == false)
		return;

	CurrentAlertMaterial = UMaterialInstanceDynamic::Create(EnemySkeletalMesh->GetMaterial(AlertMaterialIndexToChange), this);
	EnemySkeletalMesh->SetMaterial(AlertMaterialIndexToChange, CurrentAlertMaterial);
}

void AShootingEnemyPawn::ChangeParameterInAlertMaterial(float Delta)
{
	if (EnemyGunComponent->CanShootAgain() == false)
		return;

	if (bAlertAboutShoot == false || bStartAlert == false || IsValid(CurrentAlertMaterial) == false)
		return;

	AlertTimeElapsed += Delta;

	if (AlertTimeElapsed <= BeginParameterChangeAfterShootTime)
	{
		return;
	}

	CurrentAlertMaterial->SetScalarParameterValue(AlertParameterNameToChange, FMath::Lerp(0.f, EndValueForAlertParameter, AlertTimeElapsed / FinishAlertParameterChangeTime));
}

void AShootingEnemyPawn::ResetAlertMaterial()
{
	if (bAlertAboutShoot == false)
		return;

	if (CurrentAlertMaterial)
		CurrentAlertMaterial->SetScalarParameterValue(AlertParameterNameToChange, 0.f);

	AlertTimeElapsed = 0.f;
}
#pragma endregion

void AShootingEnemyPawn::SetEnemyKilledInAIController()
{
	AEnemyAiController* EnemyAIController = Cast<AEnemyAiController>(GetController());
	if (IsValid(EnemyAIController) == false)
		return;

	EnemyAIController->EnemyKilled(true);
}
