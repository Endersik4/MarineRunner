// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/GunClasses/BulletData.h"
#include "MarineRunner/EnemiesClasses/EnemyInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "EnemyPawn.generated.h"

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
		USoundBase* BoneHitSound = nullptr;

	FHitBoneType()
	{
		BoneName = "";
		DamageMultiplier = 1.f;
		bCustomSoundForHitBone = true;
		BoneHitSound = nullptr;
	}

	FHitBoneType(FName _BoneName, float _DamageMultiplier, bool _bCustomSound = true, USoundBase* _BoneHitSound = nullptr)
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

UCLASS()
class MARINERUNNER_API AEnemyPawn : public APawn, public IInteractInterface, public IEnemyInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	//Function From Interface
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius) override;

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() override { return EnemySkeletalMesh; }
	FORCEINLINE virtual class AActor* GetFocusedActor() override { return nullptr; }
	FORCEINLINE virtual void AddImpulseToPhysicsMesh(const FVector& Impulse) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* EnemySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* EnemyFloatingMovement;

	// saving/loading
	void SaveEnemySpawnedDataAtRuntime();

protected:
	bool bIsDead;
	virtual bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius);

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* EnemyIndicatorWidgetComponent;

	bool bIsRunningAway;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* EnemyCapsule;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		float LifeSpanAfterDeath = 10.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
		TArray<FHitBoneType> HitBoneTypes;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
		USoundBase* DefaultBoneHitSound;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		float MaxDistanceToObjectForBlood = 600.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		float BloodDistanceSizeMutliplier = 0.8f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		FFloatRange ClampBloodOnObjectSize = FFloatRange(40.f, 140.f);
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		float BloodFadeOutStartDelay = 3.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		float BloodFadeOutDuration = 5.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Blood On Objects")
		UMaterialInstance* BloodOnObjectDecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Footsteps")
		float TimeBetweenNextStep = 0.42f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Footsteps")
		float TimeBetweenNextStepWhileRunningAway = 0.21f;
	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* FootstepsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* FootstepsRunningAwaySound;

	//Materials
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* ShotBloodDecalMaterial;

	//Particles
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		FColor BloodColor;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UParticleSystem* EnemyBloodParticle;

	// Effects
	void SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType);
	void SpawnShotBloodDecal(const FHitResult& Hit);
	void SpawnBloodOnObjectDecal(const AActor* BulletThatHitEnemy, const FVector& HitLocation);

	// Enemy Indicator Widget
	class UEnemyIndicatorWidget* EnemyIndicatorWidget;

	//Footsteps sound
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	int32 CurrentUniqueID = 0;
	void RemoveEnemySavedDataFromSave();
};
