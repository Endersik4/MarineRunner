// Copyright Adam Bartela.All Rights Reserved


#include "EnemyPawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyIndicatorWidget.h"
#include "MarineRunner/Objects/SavedDataObject.h"

AEnemyPawn::AEnemyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Enemy Capsule"));
	RootComponent = EnemyCapsule;
	bUseControllerRotationYaw = true;
	EnemyCapsule->SetCollisionProfileName(FName(TEXT("EnemyCapsuleProf")));
	EnemyCapsule->SetSimulatePhysics(true);

	EnemyFloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("EnemyFloatingMovement"));
	
	EnemySkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("EnemySkeletalMesh"));
	EnemySkeletalMesh->SetupAttachment(EnemyCapsule);
	EnemySkeletalMesh->SetSimulatePhysics(false);
	EnemySkeletalMesh->SetCollisionProfileName(FName(TEXT("EnemySkeletalProf")));

	EnemyIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Enemy Indicator Widget Component"));
	EnemyIndicatorWidgetComponent->SetupAttachment(EnemySkeletalMesh);
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();

	EnemyIndicatorWidgetComponent->SetVisibility(false);
	EnemyIndicatorWidget = Cast<UEnemyIndicatorWidget>(EnemyIndicatorWidgetComponent->GetUserWidgetObject());
	if (IsValid(EnemyIndicatorWidget))
		EnemyIndicatorWidget->SetMaxHealth(Health);
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead == true)
		return;

	PlayFootstepsSound();

}
#pragma region ///////////////// DAMAGE ///////////////////////
void AEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
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

	KillEnemy(NewImpulseForce, NewHit, BulletActor, NewSphereRadius);
}

bool AEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius)
{
	if (Health > 0.f) 
		return false;

	if (bIsDead == false)
	{
		bIsDead = true;

		EnemySkeletalMesh->Stop();
		EnemySkeletalMesh->SetSimulatePhysics(true);
		SetLifeSpan(LifeSpanAfterDeath);

		RemoveEnemySavedDataFromSave();

		EnemyIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if (IsValid(BulletActor) == false)
		return true;

	if (NewSphereRadius != 0.f)
	{
		EnemySkeletalMesh->AddRadialImpulse(BulletActor->GetActorLocation(), NewSphereRadius, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear);
	}
	else
	{
		EnemySkeletalMesh->AddImpulse(BulletActor->GetActorForwardVector() * NewImpulseForce, NewHit.BoneName);
	}

	return true;
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
	if (bCanPlayFootstepsSound == false || bIsDead) 
		return;

	if (GetVelocity().Length() >= 0.f && GetVelocity().Length() <= 25.f)
		return;

	float FootstepsTime = TimeBetweenNextStep;
	
	if (bIsRunningAway && FootstepsRunningAwaySound)
	{
		UGameplayStatics::SpawnSoundAttached(FootstepsRunningAwaySound, EnemySkeletalMesh);
		FootstepsTime = TimeBetweenNextStepWhileRunningAway;
	}
	else if (FootstepsSound)
		UGameplayStatics::SpawnSoundAttached(FootstepsSound, EnemySkeletalMesh);


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

void AEnemyPawn::RestartData(ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData)
{
	;
}

#pragma endregion

void AEnemyPawn::AddImpulseToPhysicsMesh(const FVector& Impulse)
{
	EnemyCapsule->AddImpulse(Impulse);
}

