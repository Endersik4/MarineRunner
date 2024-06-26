// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Enemies/TypesOfEnemy/ShootingEnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"

#include "MarineRunner/Enemies/Components/EnemyGunComponent.h"
#include "MarineRunner/Enemies/ControllersAI/ShootingEnemyAIController.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

AShootingEnemyPawn::AShootingEnemyPawn()
{
	EnemyGunComponent = CreateDefaultSubobject<UEnemyGunComponent>(TEXT("Enemy Gun Component"));
}

void AShootingEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	OriginalMaxSpeed = EnemyFloatingMovement->MaxSpeed;
	OriginalShootTime = ShootTime;
	ApplyEnemyDifficulty();

	SetUpShootAlert();
}

void AShootingEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
		return;

	if (bEnemyDetectedTarget)
		FocusBonesOnPlayerWhenPlayerDetected();

	ChangeParameterInAlertMaterial(DeltaTime);
}

void AShootingEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, const EWeaponType& WeaponType, float NewSphereRadius)
{
	Super::ApplyDamage(NewDamage,NewImpulseForce, NewHit, BulletActor, WeaponType, NewSphereRadius);

}

bool AShootingEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, TObjectPtr<AActor> BulletActor, const EWeaponType& WeaponType, float NewSphereRadius)
{
	bool bKilled = Super::KillEnemy(NewImpulseForce, NewHit, BulletActor, WeaponType, NewSphereRadius);

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

	const int32 RandomPercent = FMath::RandRange(0, 100);
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
void AShootingEnemyPawn::SawTheTarget(const bool bSaw, const TObjectPtr<AActor> SeenTarget, const bool bStartAttackingTheTarget)
{
	////if (bSaw == bEnemyDetectedTarget)
	//	return;

	FocusedActor = SeenTarget;

	PlayPrepareToShootAnimation(bSaw);
	EnemyIndicatorWidgetComponent->SetVisibility(bSaw);

	if (!bStartAttackingTheTarget)
		return;

	bEnemyDetectedTarget = bSaw;

	if (bSaw)
	{
		GetWorld()->GetTimerManager().SetTimer(StartShootingHandle, this, &AShootingEnemyPawn::StartShooting, FMath::FRandRange(StartShootingRandomTimeRange.GetLowerBoundValue(), StartShootingRandomTimeRange.GetUpperBoundValue()), false);
	}
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
	{
		if (IsValid(EnableShootAnimMontage))
			EnemySkeletalMesh->GetAnimInstance()->Montage_Play(EnableShootAnimMontage);
		else
			UE_LOG(LogTemp, Warning, TEXT("Enable Shoot Anim Montage is nullptr in Shooting Enemy Pawn!"));
	}
	else
	{
		if (IsValid(DisableShootAnimMontage))
			EnemySkeletalMesh->GetAnimInstance()->Montage_Play(DisableShootAnimMontage);
		else
			UE_LOG(LogTemp, Warning, TEXT("Disable Shoot Anim Montage is nullptr in Shooting Enemy Pawn!"));
	}
}

void AShootingEnemyPawn::StartShooting()
{
	if (bIsDead)
		return;

	bStartAlert = true;
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &AShootingEnemyPawn::Shoot, ShootTime, true, 0.f);
}

void AShootingEnemyPawn::Shoot()
{
	if (!EnemyGunComponent->CanShootAgain())
		return;

	ResetAlertMaterial();

	EnemyGunComponent->Shoot();

	if (IsValid(ShootAnimMontage))
		EnemySkeletalMesh->GetAnimInstance()->Montage_Play(ShootAnimMontage);
	else
		UE_LOG(LogTemp, Warning, TEXT("Shoot Anim Montage is nullptr in Shooting Enemy Pawn!"));
}

const FRotator AShootingEnemyPawn::FocusBoneOnPlayer(const FName BoneName, const bool bLookStraight)
{
	FRotator BoneRotation = FRotator(0.f);
	const FRotator& FoundRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneName),
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

	if (!IsValid(EnemySkeletalMesh->GetMaterial(AlertMaterialIndexToChange)))
		return;

	CurrentAlertMaterial = UMaterialInstanceDynamic::Create(EnemySkeletalMesh->GetMaterial(AlertMaterialIndexToChange), this);
	if (!IsValid(CurrentAlertMaterial))
		return;

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
	if (!ensureMsgf(IsValid(GetController()), TEXT("Enemy Controller is nullptr in Shooting Enemy Pawn")))
		return;

	TObjectPtr<AShootingEnemyAIController> EnemyAIController = Cast<AShootingEnemyAIController>(GetController());
	if (!IsValid(EnemyAIController))
		return;

	EnemyAIController->EnemyKilled(true);
}

void AShootingEnemyPawn::ApplyEnemyDifficulty()
{
	TObjectPtr<UGameInstance> GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!IsValid(GameInstance))
		return;

	TObjectPtr<UMarineRunnerGameInstance> MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(GameInstance);
	if (!IsValid(MarineRunnerGameInstance))
		return;

	int32 GameDifficultyNumber = MarineRunnerGameInstance->FindSavedValueAccordingToName(MarineRunnerGameInstance->GetGameDifficultySavedFieldName());
	if (GameDifficultyNumber >= MarineRunnerGameInstance->GetAllGameDifficulties().Num())
		return;

	const float& EnemiesDifficultyPercent = MarineRunnerGameInstance->GetAllGameDifficulties()[GameDifficultyNumber].EnemiesDifficultyPercent;
	Health = OriginalHealth * EnemiesDifficultyPercent;
	EnemyGunComponent->ApplyWeaponDifficulty(EnemiesDifficultyPercent);
	EnemyFloatingMovement->MaxSpeed = OriginalMaxSpeed * EnemiesDifficultyPercent;
	ShootTime = OriginalShootTime / EnemiesDifficultyPercent;

	SetUpEnemyHealthIndicatorWidgetComponent();
}