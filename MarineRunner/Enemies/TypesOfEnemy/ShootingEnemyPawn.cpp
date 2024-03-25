// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Enemies/TypesOfEnemy/ShootingEnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"

#include "MarineRunner/Enemies/Components/EnemyGunComponent.h"
#include "MarineRunner/Enemies/ControllersAI/ShootingEnemyAIController.h"

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
	if (bIsDead)
		return;

	Super::Tick(DeltaTime);

	if (bEnemyDetectedTarget)
		FocusBonesOnPlayerWhenPlayerDetected();

	ChangeParameterInAlertMaterial(DeltaTime);
}

void AShootingEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	Super::ApplyDamage(NewDamage,NewImpulseForce, NewHit, BulletActor, NewSphereRadius);

	KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);
}

bool AShootingEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, TObjectPtr<AActor> BulletActor, float NewSphereRadius)
{
	bool bKilled = Super::KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);

	if (bKilled)
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
	if (Health > MaxEnemyHealthForRunAway || !bCanEnemyRunAway)
		return false;

	int32 RandomPercent = FMath::RandRange(0, 100);
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

	if (bEnemyDetectedTarget)
		PlayPrepareToShootAnimation(false);

	SawTheTarget(false);
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
	GetWorld()->GetTimerManager().ClearTimer(StartShootingHandle);
	ResetAlertMaterial();

	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
}
#pragma region ////////////// ENEMY SEE PLAYER //////////////
void AShootingEnemyPawn::SawTheTarget(bool bSaw, TObjectPtr<AActor> SeenTarget, bool bStartAttackingTheTarget)
{
	if (bSaw == bEnemyDetectedTarget)
		return;

	FocusedActor = SeenTarget;

	PlayPrepareToShootAnimation(bSaw);
	EnemyIndicatorWidgetComponent->SetVisibility(bSaw);

	if (!bStartAttackingTheTarget)
		return;

	bEnemyDetectedTarget = bSaw;

	const float& StartShootingTime = TimeToStartShooting + FMath::FRandRange(StartShootingRandomTimeRange.GetLowerBoundValue(), StartShootingRandomTimeRange.GetUpperBoundValue());
	if (bSaw)
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AShootingEnemyPawn::StartShooting, StartShootingTime, false);
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
		bStartAlert = false;
	}
}

void AShootingEnemyPawn::PlayPrepareToShootAnimation(bool bTargetWasDetected)
{
	if (!IsValid(EnableShootAnimMontage)|| !IsValid(DisableShootAnimMontage))
		return;

	if (bTargetWasDetected)
		EnemySkeletalMesh->GetAnimInstance()->Montage_Play(EnableShootAnimMontage);
	else
		EnemySkeletalMesh->GetAnimInstance()->Montage_Play(DisableShootAnimMontage);
}

void AShootingEnemyPawn::StartShooting()
{
	if (bIsDead)
		return;

	bStartAlert = true;
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &AShootingEnemyPawn::Shoot, ShootTime, true);
}

void AShootingEnemyPawn::Shoot()
{
	if (!EnemyGunComponent->CanShootAgain())
		return;

	ResetAlertMaterial();

	EnemyGunComponent->Shoot();

	if (IsValid(ShootAnimMontage))
		EnemySkeletalMesh->GetAnimInstance()->Montage_Play(ShootAnimMontage);
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
	if (!bAlertAboutShoot)
		return;

	CurrentAlertMaterial = UMaterialInstanceDynamic::Create(EnemySkeletalMesh->GetMaterial(AlertMaterialIndexToChange), this);
	EnemySkeletalMesh->SetMaterial(AlertMaterialIndexToChange, CurrentAlertMaterial);
}

void AShootingEnemyPawn::ChangeParameterInAlertMaterial(float Delta)
{
	if (!EnemyGunComponent->CanShootAgain())
		return;

	if (!bAlertAboutShoot|| !bStartAlert || !IsValid(CurrentAlertMaterial))
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
	if (!bAlertAboutShoot)
		return;

	if (CurrentAlertMaterial)
		CurrentAlertMaterial->SetScalarParameterValue(AlertParameterNameToChange, 0.f);

	AlertTimeElapsed = 0.f;
}
#pragma endregion

void AShootingEnemyPawn::SetEnemyKilledInAIController()
{
	TObjectPtr<AShootingEnemyAIController> EnemyAIController = Cast<AShootingEnemyAIController>(GetController());
	if (!IsValid(EnemyAIController))
		return;

	EnemyAIController->EnemyKilled(true);
}
