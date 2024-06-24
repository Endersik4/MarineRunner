// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/DamageInterface.h"
#include "MarineRunner/Weapon/Gun/Bullet/BulletData.h"
#include "MarineRunner/Enemies/Interfaces/EnemyGunInterface.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "EnemyBasePawn.generated.h"

USTRUCT(BlueprintType)
struct FHitBoneType {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle))
	bool bCustomSoundForHitBone = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCustomSoundForHitBone"))
	TObjectPtr<USoundBase> BoneHitSound = nullptr;

	FHitBoneType()
	{
		BoneName = "";
		DamageMultiplier = 1.f;
		bCustomSoundForHitBone = true;
		BoneHitSound = nullptr;
	}

	FHitBoneType(FName _BoneName, float _DamageMultiplier, bool _bCustomSound = true, TObjectPtr<USoundBase> _BoneHitSound = nullptr)
	{
		BoneName = _BoneName;
		DamageMultiplier = _DamageMultiplier;
		bCustomSoundForHitBone = _bCustomSound;
		BoneHitSound = _BoneHitSound;
	}


	bool operator==(const FHitBoneType& OtherHitBone)
	{
		return BoneName == OtherHitBone.BoneName;
	}
	bool operator==(const FName& OtherHitBoneName)
	{
		return BoneName == OtherHitBoneName;
	}
};

USTRUCT(BlueprintType)
struct FWoundDecal
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EWeaponType> WoundFromWeaponType;
	UPROPERTY(EditAnywhere)
	FVector WoundDecalSize = FVector();
	UPROPERTY(EditAnywhere)
	FFloatRange WoundRandomSizeMultiplierRange = FFloatRange();
	// It was added to the decal of gunshot wounds after spawn, because sometimes the decal is not visible on the body if it was moved slightly.
	UPROPERTY(EditAnywhere)
	float AdditionalWoundSize_X = 0.f;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInstance> WoundDecalMaterial = nullptr;
	UPROPERTY(EditAnywhere)
	float WoundFadeOutDelay = 0.f;
	UPROPERTY(EditAnywhere)
	float WoundFadeOutDuration = 0.f;

	const bool operator==(const EWeaponType& otherWeaponType) const
	{
		return WoundFromWeaponType == otherWeaponType;
	}

	FWoundDecal()
	{
		WoundFromWeaponType = EWeaponType::EWT_Gun;
		WoundDecalSize = FVector();
		WoundRandomSizeMultiplierRange = FFloatRange();
		AdditionalWoundSize_X = 0.f;
		WoundDecalMaterial = nullptr;
		WoundFadeOutDelay = 0.f;
		WoundFadeOutDuration = 0.f;
	}
};

UCLASS()
class MARINERUNNER_API AEnemyPawn : public APawn, public IDamageInterface, public IEnemyInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

	//Function From IDamageInterface
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, const EWeaponType& WeaponType, float NewSphereRadius = 0.f) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ISaveCustomDataInterface
	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Effects
	void SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType);
	void SpawnGunshotWoundDecal(const FHitResult& Hit, const TObjectPtr<class USkeletalMeshComponent> SkeletalMeshToSpawnOn, const EWeaponType& WeaponType);
	void SpawnBloodOnObjectDecal(TObjectPtr<const AActor> BulletThatHitEnemy, const FVector& HitLocation);
	FHitBoneType* GetHitBoneType(const FName& BoneNameToFind);

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() override { return EnemySkeletalMesh; }
	FORCEINLINE virtual class AActor* GetFocusedActor() override { return nullptr; }
	FORCEINLINE virtual void AddImpulseToPhysicsMesh(const FVector& Impulse) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> EnemySkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UFloatingPawnMovement> EnemyFloatingMovement = nullptr;

	// saving/loading
	void SaveEnemySpawnedDataAtRuntime();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetEnemyHealth(float& NewHealth) { Health = NewHealth; }

	FORCEINLINE TObjectPtr<class UDismemberEnemyComponent> GetDismemberEnemyComponent() { return DismemberEnemyComponent; }
	FORCEINLINE const FColor& GetBloodColor() { return BloodColor; }

protected:
	UPROPERTY(Transient)
	bool bIsDead = false;

	virtual bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, TObjectPtr<AActor> BulletActor, const EWeaponType& WeaponType, float NewSphereRadius);

	UPROPERTY(Transient, EditAnywhere, Category = "Setting Enemy")
	float Health = 100.f;
	UPROPERTY(Transient)
	float OriginalHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> EnemyIndicatorWidgetComponent = nullptr;

	UPROPERTY(Transient)
	bool bIsRunningAway = false;

	void SetUpEnemyHealthIndicatorWidgetComponent();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UCapsuleComponent> EnemyCapsule = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UDismemberEnemyComponent> DismemberEnemyComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
	float LifeSpanAfterDeath = 20.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
	TArray<FHitBoneType> HitBoneTypes = { FHitBoneType() };
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
	TObjectPtr<USoundBase> DefaultBoneHitSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	float MaxDistanceToObjectForBlood = 900.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	float BloodDistanceSizeMutliplier = 0.8f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	FFloatRange ClampBloodOnObjectSize = FFloatRange(40.f, 140.f);
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	float BloodFadeOutStartDelay = 10.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	float BloodFadeOutDuration = 5.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
	TObjectPtr<UMaterialInstance> BloodOnObjectDecalMaterial = nullptr;
	// It was added to the Blood On Object Decal after spawn, because sometimes the decal is not visible on the body if it was moved slightly.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Blood On Objects")
	float AdditionalBloodOnObjectSize_X = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Gunshot wound")
	TArray<FWoundDecal> AllWoundDecals;

	UPROPERTY(EditAnywhere, Category = "Footsteps")
	float TimeBetweenNextStep = 0.42f;
	UPROPERTY(EditAnywhere, Category = "Footsteps")
	float TimeBetweenNextStepWhileRunningAway = 0.21f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	FFloatRange VelocityRangeToActivateFootsteps = FFloatRange(0.f, 25.f);
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	TObjectPtr<USoundBase> FootstepsSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	TObjectPtr<USoundBase> FootstepsRunningAwaySound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Blood Spray")
	FRotator EnemyBloodParticleRotationOffset = FRotator(90.f, 0.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Blood Spray")
	FColor BloodColor = FColor::Red;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Blood Spray")
	FName BloodColorParameterName = FName(TEXT("ColorOfBlood"));
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Blood Spray")
	TObjectPtr<UParticleSystem> EnemyBloodParticle = nullptr;

	// Enemy Indicator Widget
	UPROPERTY(Transient)
	TObjectPtr<class UEnemyIndicatorWidget> EnemyIndicatorWidget = nullptr;

	//Footsteps sound
	void PlayFootstepsSound();
	UPROPERTY(Transient)
	bool bCanPlayFootstepsSound = true;
	UPROPERTY(Transient)
	FTimerHandle FootstepsHandle;
	FORCEINLINE void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;
	void RemoveEnemySavedDataFromSave();

};
