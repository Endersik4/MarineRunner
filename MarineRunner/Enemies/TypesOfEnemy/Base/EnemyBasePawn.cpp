// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "Components/WidgetComponent.h"

#include "MarineRunner/Enemies/Widgets/EnemyHealthIndicatorWidget.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "MarineRunner/Enemies/Components/DismemberEnemyComponent.h"

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

	DismemberEnemyComponent = CreateDefaultSubobject<UDismemberEnemyComponent>(TEXT("Dismember Enemy Component"));

	EnemyIndicatorWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Enemy Indicator Widget Component"));
	EnemyIndicatorWidgetComponent->SetupAttachment(EnemySkeletalMesh);
	EnemyIndicatorWidgetComponent->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

// Called when the game starts or when spawned
void AEnemyPawn::BeginPlay()
{
	Super::BeginPlay();
	OriginalHealth = Health;

	SetUpEnemyHealthIndicatorWidgetComponent();
}

// Called every frame
void AEnemyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead)
		return;
	
	PlayFootstepsSound();

}
#pragma region ///////////////// DAMAGE ///////////////////////
void AEnemyPawn::ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, const EWeaponType& WeaponType, float NewSphereRadiusToApplyDamage)
{
	const FHitBoneType* FoundBoneForExtraDamage = GetHitBoneType(NewHit.BoneName);
	if (FoundBoneForExtraDamage)
	{
		NewDamage *= FoundBoneForExtraDamage->DamageMultiplier;
	}

	Health -= NewDamage;
	SpawnEffectsForImpact(NewHit, FoundBoneForExtraDamage);
	SpawnGunshotWoundDecal(NewHit, EnemySkeletalMesh, WeaponType);
	SpawnBloodOnObjectDecal(BulletActor, NewHit.Location);

	if (IsValid(EnemyIndicatorWidget))
	{
		EnemyIndicatorWidget->SetCurrentHealthInHealthBar(Health);
	}

	KillEnemy(NewImpulseForce, NewHit, BulletActor, WeaponType, NewSphereRadiusToApplyDamage);
}

bool AEnemyPawn::KillEnemy(float NewImpulseForce, const FHitResult& NewHit, TObjectPtr<AActor> BulletActor, const EWeaponType& WeaponType, float NewSphereRadiusToApplyDamage)
{
	if (Health > 0.f) 
		return false;

	if (!bIsDead)
	{
		bIsDead = true;

		EnemyCapsule->SetSimulatePhysics(false);
		EnemyCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		EnemySkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		EnemySkeletalMesh->Stop();
		EnemySkeletalMesh->SetSimulatePhysics(true);
		SetLifeSpan(LifeSpanAfterDeath);

		RemoveEnemySavedDataFromSave();

		EnemyIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	// If the radius of the sphere is 0, the bullet did not use the radial sphere to deal damage.
	if (NewSphereRadiusToApplyDamage == 0.f && IsValid(BulletActor))
	{
		EnemySkeletalMesh->AddImpulse(BulletActor->GetActorForwardVector() * NewImpulseForce, NewHit.BoneName);
	}
	else
	{
		EnemySkeletalMesh->AddRadialImpulse(NewHit.TraceStart, NewSphereRadiusToApplyDamage, NewImpulseForce, ERadialImpulseFalloff::RIF_Linear);
	}

	DismemberEnemyComponent->DismemberLimb(EnemySkeletalMesh, NewHit, NewImpulseForce, BloodColor, WeaponType, NewSphereRadiusToApplyDamage);

	return true;
}
#pragma endregion 

#pragma region ///////////// EFFECTS ////////////////////
void AEnemyPawn::SpawnGunshotWoundDecal(const FHitResult& Hit, const TObjectPtr<class USkeletalMeshComponent> SkeletalMeshToSpawnOn, const EWeaponType& WeaponType)
{
	FWoundDecal* FoundWoundDecal = AllWoundDecals.FindByKey(WeaponType);
	if (!FoundWoundDecal)
		return;

	if (!IsValid(FoundWoundDecal->DecalMaterial))
		return;

	const float& RandomSizeMultiplier = FMath::FRandRange(FoundWoundDecal->WoundRandomSizeMultiplierRange.GetLowerBoundValue(), FoundWoundDecal->WoundRandomSizeMultiplierRange.GetUpperBoundValue());
	const FVector& GunshotWoundSize = FoundWoundDecal->DecalSize * RandomSizeMultiplier;

	UCustomDecalUtility::SpawnDecalAttached(*FoundWoundDecal, SkeletalMeshToSpawnOn, Hit.BoneName, Hit.Location, Hit.ImpactNormal.Rotation(), GunshotWoundSize);
}

void AEnemyPawn::PlayFootstepsSound()
{
	if (!bCanPlayFootstepsSound || bIsDead) 
		return;

	if (GetVelocity().Length() >= VelocityRangeToActivateFootsteps.GetLowerBoundValue() 
		&& GetVelocity().Length() <= VelocityRangeToActivateFootsteps.GetUpperBoundValue())
		return;

	float FootstepsTime = TimeBetweenNextStep;
	
	if (bIsRunningAway)
	{
		if (IsValid(FootstepsRunningAwaySound))
			UGameplayStatics::SpawnSoundAttached(FootstepsRunningAwaySound, EnemySkeletalMesh);
		else
			UE_LOG(LogTemp, Warning, TEXT("Footsteps Running Away Sound is nullptr in EnemyBasePawn!"));

		FootstepsTime = TimeBetweenNextStepWhileRunningAway;
	}
	else
	{
		if (IsValid(FootstepsSound))
			UGameplayStatics::SpawnSoundAttached(FootstepsSound, EnemySkeletalMesh);
		else
			UE_LOG(LogTemp, Warning, TEXT("Footsteps Sound is nullptr in EnemyBasePawn!"));
	}

	bCanPlayFootstepsSound = false;
	GetWorldTimerManager().SetTimer(FootstepsHandle, this, &AEnemyPawn::SetCanPlayFootstepsSound, FootstepsTime, false);
}

void AEnemyPawn::SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType)
{
	if (IsValid(EnemyBloodParticle))
	{
		const FRotator& EnemyBloodParticleRotation = Hit.ImpactNormal.Rotation() - EnemyBloodParticleRotationOffset;

		TObjectPtr<UParticleSystemComponent> SpawnedParticle = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemyBloodParticle, Hit.ImpactPoint, EnemyBloodParticleRotation);
		if (IsValid(SpawnedParticle))
			SpawnedParticle->SetColorParameter(BloodColorParameterName, BloodColor);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Enemy Blood Particle is nullptr in EnemyBasePawn!"));

	if (PtrHitBoneType)
	{
		if (PtrHitBoneType->bCustomSoundForHitBone == true && PtrHitBoneType->BoneHitSound)
		{
			if (IsValid(PtrHitBoneType->BoneHitSound))
				UGameplayStatics::PlaySound2D(GetWorld(), PtrHitBoneType->BoneHitSound);
			else
				UE_LOG(LogTemp, Warning, TEXT("Bone Hit Sound is nullptr in EnemyBasePawn!"));

			return;
		}
	}

	if (IsValid(DefaultBoneHitSound)) 
		UGameplayStatics::PlaySound2D(GetWorld(), DefaultBoneHitSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Default Bone Hit Sound is nullptr in EnemyBasePawn!"));
}

void AEnemyPawn::SpawnBloodOnObjectDecal(TObjectPtr<const AActor> BulletThatHitEnemy, const FVector& HitLocation)
{
	if (!IsValid(BloodOnObject.DecalMaterial) || !IsValid(BulletThatHitEnemy))
		return;

	FHitResult ObjectToSpawnBloodOnHitResult;
	const FVector& StartRaycast = HitLocation;
	const FVector EndRaycast = HitLocation + BulletThatHitEnemy->GetActorForwardVector() * MaxDistanceToObjectForBlood;
	const bool bObjectHit = GetWorld()->LineTraceSingleByChannel(ObjectToSpawnBloodOnHitResult, StartRaycast, EndRaycast, ECC_GameTraceChannel5);

	if (!bObjectHit)
		return;

	const float MultiplierAccordingToDistance = FVector::Distance(StartRaycast, ObjectToSpawnBloodOnHitResult.ImpactPoint) / MaxDistanceToObjectForBlood;
	const FVector DecalSizeAccordingToDistance = BloodOnObject.DecalSize * MultiplierAccordingToDistance;

	UCustomDecalUtility::SpawnDecalAtLocation(BloodOnObject, GetWorld(), ObjectToSpawnBloodOnHitResult.Location, ObjectToSpawnBloodOnHitResult.ImpactNormal.Rotation(), DecalSizeAccordingToDistance);
}
#pragma endregion

#pragma region 
//////////// SAVE/LOAD /////////////
void AEnemyPawn::RemoveEnemySavedDataFromSave()
{
	if (CurrentUniqueID == 0)
		return;

	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
		return;

	SavedDataObject->RemoveCustomSaveData(CurrentUniqueID);
}

void AEnemyPawn::SaveEnemySpawnedDataAtRuntime()
{
	TObjectPtr<ASavedDataObject> SavedDataObject = Cast<ASavedDataObject>(UGameplayStatics::GetActorOfClass(GetWorld(), ASavedDataObject::StaticClass()));

	if (!IsValid(SavedDataObject))
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
	if (!IsValid(SavedDataObject))
		return;

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

FHitBoneType* AEnemyPawn::GetHitBoneType(const FName& BoneNameToFind)
{
	return HitBoneTypes.FindByKey(BoneNameToFind);
}

void AEnemyPawn::AddImpulseToPhysicsMesh(const FVector& Impulse)
{
	EnemyCapsule->AddImpulse(Impulse);
}

void AEnemyPawn::SetUpEnemyHealthIndicatorWidgetComponent()
{
	if (IsValid(EnemyIndicatorWidget))
	{
		EnemyIndicatorWidget->SetMaxHealth(Health);
		EnemyIndicatorWidget->RestartEnemyHealthBar();
		return;
	}

	EnemyIndicatorWidgetComponent->SetVisibility(false);
	if (!ensureMsgf(IsValid(EnemyIndicatorWidgetComponent->GetUserWidgetObject()), TEXT("User Widget Object in Enemy Indicator Widget Component is nullptr in EnemyBasePawn")))
		return;

	EnemyIndicatorWidget = Cast<UEnemyIndicatorWidget>(EnemyIndicatorWidgetComponent->GetUserWidgetObject());
	if (!IsValid(EnemyIndicatorWidget))
		return;
	
	EnemyIndicatorWidget->SetMaxHealth(Health);
}