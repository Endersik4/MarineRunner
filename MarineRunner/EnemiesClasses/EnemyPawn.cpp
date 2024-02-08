// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"

#include "MarineRunner/GunClasses/Bullet.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/EnemiesClasses/EnemyAiController.h"
#include "MarineRunner/EnemiesClasses/EnemyGunComponent.h"
#include "MarineRunner/EnemiesClasses/EnemyIndicatorWidget.h"
#include "MarineRunner/Objects/SavedDataObject.h"

AEnemyPawn::AEnemyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleColl = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	RootComponent = CapsuleColl;
	bUseControllerRotationYaw = true;
	CapsuleColl->SetCollisionProfileName(FName(TEXT("EnemyCapsuleProf")));

	EnemyFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("EnemyFloatingMovement"));
	
	EnemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemySkeletalMesh"));
	EnemySkeletalMesh->SetupAttachment(CapsuleColl);
	EnemySkeletalMesh->SetSimulatePhysics(false);
	EnemySkeletalMesh->SetCollisionProfileName(FName(TEXT("EnemySkeletalProf")));

	EnemyGunComponent = CreateDefaultSubobject<UEnemyGunComponent>(TEXT("Enemy Gun Component"));

	EnemyIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Enemy Indicator Widget Component"));
	EnemyIndicatorWidgetComponent->SetupAttachment(EnemySkeletalMesh);

}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	SetUpMarinePawn();
	SetUpEnemyAIController();

	EnemyIndicatorWidget = Cast<UEnemyIndicatorWidget>(EnemyIndicatorWidgetComponent->GetUserWidgetObject());
	if (IsValid(EnemyIndicatorWidget))
	{
		EnemyIndicatorWidget->SetMaxHealth(Health);
	}
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead == true)
		return;

	CheckIfEnemySeePlayer();
	PlayFootstepsSound();
}

// Called to bind functionality to input
void AEnemyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

#pragma region ///////////////// SHOOT //////////////////
//Shoot function is executed in ShootAndMove.cpp
void AEnemyPawn::PredictWhereToShoot()
{
	PlayerCameraLocation = MarinePawn->GetCamera()->GetComponentLocation();

	if (MarinePawn->GetInputAxisValue("Right") == 1.f)
	{
		PlayerCameraLocation += MarinePawn->GetCamera()->GetRightVector() * 100.f;
	}
	else if (MarinePawn->GetInputAxisValue("Right") == -1.f)
	{
		PlayerCameraLocation -= MarinePawn->GetCamera()->GetRightVector() * 100.f;
	}

	if (bShouldAlsoPredictVertical == false) return;

	PlayerCameraLocation.Z += FVector::Distance(GetActorLocation(), MarinePawn->GetActorLocation()) / 10;
}
#pragma endregion

#pragma region ///////////////// DAMAGE ///////////////////////
void AEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (bCanEnemyBeKilled == false)
		return;

	FHitBoneType* FoundBone = HitBoneTypes.FindByKey(NewHit.BoneName);
	if (FoundBone)
	{
		NewDamage *= FoundBone->DamageMultiplier;
	}

	Health -= NewDamage;
	SpawnEffectsForImpact(NewHit, FoundBone);
	SpawnShotBloodDecal(NewHit);
	SpawnBloodOnObjectDecal(BulletActor, NewHit.Location);

	if (IsValid(EnemyIndicatorWidget))
	{
		EnemyIndicatorWidget->SetCurrentHealthInHealthBar(Health);
	}

	bool bEnemyKilled = KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);
	if (bEnemyKilled == true) return;

	bool bEnemyIsRunningAway = EnemyRunAway();
	if (bEnemyIsRunningAway == true) return;

	AlertEnemyAboutPlayer();
}

bool AEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (Health > 0.f) return false;

	if (bIsDead == false)
	{
		EnemySkeletalMesh->Stop();
		EnemySkeletalMesh->SetSimulatePhysics(true);
		SetIsDead(true);
		EnemyAIController->AddEnemyToDetected(false);
		SetLifeSpan(LifeSpanAfterDeath);
		RemoveEnemySavedDataFromSave();

		EnemyIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(BulletActor) == false)
		return true;

	if (NewSphereRadius != 0.f)
	{
		EnemySkeletalMesh->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce * 10.f, ERadialImpulseFalloff::RIF_Linear);
	}
	else
	{
		EnemySkeletalMesh->AddImpulse(BulletActor->GetActorForwardVector() * NewImpulseForce * 10.f, NewHit.BoneName);
	}

	return true;
}

bool AEnemyPawn::EnemyRunAway()
{
	if (Health > 10.f || bCanEnemyRunAway == false) return false;

	float ShouldRunAwayRandom = FMath::FRandRange(0.f, 100.f);
	if (ShouldRunAwayRandom <= 30.f)
	{
		ShouldRunAway();
		return true;
	}

	return false;
}

void AEnemyPawn::AlertEnemyAboutPlayer()
{
	if (!EnemyAIController) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == true) return;

	FVector PlayerLocation = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	float FoundRotationYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerLocation).Yaw;

	FRotator EnemyRotation = GetActorRotation();
	EnemyRotation.Yaw = FoundRotationYaw;
	SetActorRotation(EnemyRotation);
}
#pragma endregion 

#pragma region ///////////// EFFECTS ////////////////////
void AEnemyPawn::SpawnShotBloodDecal(const FHitResult& Hit)
{
	if (!ShotBloodDecalMaterial) return;

	FVector Size = FVector(FMath::FRandRange(8.f,18.f));
	FRotator Rotation = Hit.ImpactNormal.Rotation();
	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAttached(ShotBloodDecalMaterial, Size, EnemySkeletalMesh, Hit.BoneName, Hit.Location, Rotation, EAttachLocation::KeepWorldPosition, 10.f);
	if (IsValid(SpawnedDecal))
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
		SpawnedDecal->DecalSize.X += 20.f;
	}
}

void AEnemyPawn::PlayFootstepsSound()
{
	if (!FootstepsSound) return;
	if (GetVelocity().Length() < 1.f || bCanPlayFootstepsSound == false || bIsDead) return;

	float FootstepsTime = 0.42f;
	if (bIsRunningAway)
	{
		if (FootstepsRunningAwaySound) UGameplayStatics::SpawnSoundAttached(FootstepsRunningAwaySound, EnemySkeletalMesh);
		FootstepsTime = 0.2f;
	}
	else UGameplayStatics::SpawnSoundAttached(FootstepsSound, EnemySkeletalMesh);

	bCanPlayFootstepsSound = false;
	GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AEnemyPawn::SetCanPlayFootstepsSound, FootstepsTime, false);
}

void AEnemyPawn::SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType)
{
	if (EnemyBloodParticle)
	{
		FRotator Rotation = Hit.ImpactNormal.Rotation() - FRotator(90.f, 0.f, 0.f);
		UParticleSystemComponent* SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyBloodParticle, Hit.ImpactPoint, Rotation);
		SpawnedParticle->SetColorParameter(TEXT("ColorOfBlood"), BloodColor);
	}

	if (PtrHitBoneType)
	{
		if (PtrHitBoneType->bCustomSoundForHitBone == true && PtrHitBoneType->BoneHitSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), PtrHitBoneType->BoneHitSound);
			return;
		}
	}

	if (DefaultBoneHitSound) 
		UGameplayStatics::PlaySound2D(GetWorld(), DefaultBoneHitSound);

}

void AEnemyPawn::SpawnBloodOnObjectDecal(const AActor* BulletThatHitEnemy, const FVector& HitLocation)
{
	if (BloodOnObjectDecalMaterial == nullptr || IsValid(BulletThatHitEnemy) == false)
		return;

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, HitLocation, HitLocation + BulletThatHitEnemy->GetActorForwardVector() * MaxDistanceToObjectForBlood, ECC_GameTraceChannel5);

	if (bHit == false)
		return;

	FVector DecalSizeAccordingToDistance = FVector(FMath::Clamp(HitResult.Distance * BloodDistanceSizeMutliplier, ClampBloodOnObjectSize.GetLowerBoundValue(), ClampBloodOnObjectSize.GetUpperBoundValue()));
	DecalSizeAccordingToDistance.X = 0.03f;
	UDecalComponent* SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BloodOnObjectDecalMaterial, DecalSizeAccordingToDistance, HitResult.Location, (HitResult.Normal * -1.f).Rotation());

	if (IsValid(SpawnedDecal))
	{
		SpawnedDecal->SetFadeScreenSize(0.f);
		SpawnedDecal->DecalSize.X += 20.f;
		SpawnedDecal->SetFadeOut(BloodFadeOutStartDelay, BloodFadeOutDuration);
	}
}
#pragma endregion

#pragma region ////////////// ENEMY SEE PLAYER //////////////
bool AEnemyPawn::ConditionsForEnemyToSeePlayer()
{
	if (!EnemyAIController || bIsDead == true || !MarinePawn || bIsRunningAway) return false;
	if (EnemyAIController->GetDoEnemySeePlayer() == false) return false;

	return true;
}

void AEnemyPawn::CheckIfEnemySeePlayer()
{
	if (ConditionsForEnemyToSeePlayer() == false) return;

	PredictWhereToShoot();
	FocusBonesOnPlayerWhenPlayerDetected();
}
#pragma endregion

FRotator AEnemyPawn::FocusBoneOnPlayer(FName BoneName, bool bLookStraight)
{
	FRotator BoneRotation;
	FRotator FoundRotation = UKismetMathLibrary::FindLookAtRotation(EnemySkeletalMesh->GetSocketLocation(BoneName), 
		(bLookStraight) ? MarinePawn->GetCamera()->GetComponentLocation() : PlayerCameraLocation);
	BoneRotation.Roll = FoundRotation.Pitch * -1.f;
	BoneRotation.Yaw = FoundRotation.Yaw - GetActorRotation().Yaw;

	return BoneRotation;
}

void AEnemyPawn::ShouldRunAway()
{
	bIsRunningAway = true;
	SetShouldRunningAwayInAnimBP();
	EnemyAIController->RunAway();
}

#pragma region /////////// SETTERS ////////////////
void AEnemyPawn::SetIsDead(bool bNewDead)
{
	bIsDead = bNewDead;
	EnemyAIController->KillEnemy();
}

void AEnemyPawn::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!MarinePawn)
	{
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN CAST ERROR IN ENEMY PAWN"));
	}
}

void AEnemyPawn::SetUpEnemyAIController()
{
	EnemyAIController = Cast<AEnemyAiController>(GetController());
	if (IsValid(EnemyAIController) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("ENEMY AI CONTROLER CAST ERROR IN ENEMY PAWN"));
		return;
	}
	EnemyAIController->SetAIVariables();

}
#pragma endregion

#pragma region //////////// SAVE/LOAD /////////////
void AEnemyPawn::RemoveEnemySavedDataFromSave()
{
	if (CurrentUniqueID == 0)
		return;

	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	SavedDataObject->RemoveCustomSaveData(CurrentUniqueID);
}

void AEnemyPawn::SaveEnemySpawnedDataAtRuntime()
{
	ASavedDataObject* SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (IsValid(SavedDataObject) == false)
		return;

	FCustomDataSaved ItemSpawnedData = FCustomDataSaved(ESavedDataState::ESDS_SpawnObject, GetClass(), FTransform(GetActorRotation(), GetActorLocation()), Health);
	ItemSpawnedData.ObjectState = 1;
	ItemSpawnedData.ObjectToSaveData = this;

	CurrentUniqueID = SavedDataObject->CreateUniqueIDForObject();
	SavedDataObject->AddCustomSaveData(CurrentUniqueID, ItemSpawnedData);
}

void AEnemyPawn::LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	CurrentUniqueID = IDkey;
	Health = SavedCustomData.ValueToSave;
	if (IsValid(EnemyIndicatorWidget))
	{
		EnemyIndicatorWidget->SetCurrentHealthInHealthBar(Health);
	}
}

void AEnemyPawn::SaveData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	SavedDataObject->RemoveCustomSaveData(IDkey);
	FCustomDataSaved UpdatedData = SavedCustomData;
	UpdatedData.ObjectTransform = FTransform(GetActorRotation(), GetActorLocation());
	UpdatedData.ValueToSave = Health;
	UpdatedData.ObjectToSaveData = this;
	SavedDataObject->AddCustomSaveData(IDkey, UpdatedData);
}
#pragma endregion

void AEnemyPawn::AddImpulseToPhysicsMesh(const FVector& Impulse)
{
	CapsuleColl->AddImpulse(Impulse);
}

void AEnemyPawn::ShootAgain(bool& bShoot)
{
	bShoot = EnemyGunComponent->CanShootAgain();
}

void AEnemyPawn::Reload()
{
	EnemyGunComponent->Reload();
}

void AEnemyPawn::Shoot()
{
	EnemyGunComponent->Shoot();
}
