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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 
	//Function From Interface
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius) override;

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() override { return EnemySkeletalMesh; }
	FORCEINLINE virtual class AActor* GetFocusedActor() override { return FocusedActor; }
	FORCEINLINE virtual void AddImpulseToPhysicsMesh(const FVector& Impulse) override;

	UFUNCTION(BlueprintImplementableEvent)
		void SetShouldRunningAwayInAnimBP();

	UFUNCTION(BlueprintImplementableEvent)
		void RestoreBonesToInitialRotation();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayPrepareToShootAnimation(bool bTargetWasDetected);
	UFUNCTION(BlueprintImplementableEvent)
		void PlayShootMontageAnimation();

	//Had to get AnimInstance of AnimationBlueprint so in blueprint you are setting what bone you want to looking at the player
	UFUNCTION(BlueprintImplementableEvent)
		void FocusBonesOnPlayerWhenPlayerDetected();

	// Bone Rotation - Bone will be looking at the player when he is detected.
	// bLookStraight means whether Bone should look directly at the player's camera or with some margin
	UFUNCTION(BlueprintCallable)
		FRotator FocusBoneOnPlayer(FName BoneName, bool bLookStraight);

	UFUNCTION(BlueprintPure)
		float GetSpeedOfEnemyWhenIsRunningAway() const { return SpeedOfEnemyWhenIsRunningAway; }

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* EnemySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* EnemyFloatingMovement;

	// saving/loading
	void SaveEnemySpawnedDataAtRuntime();

	void SawTheTarget(bool bSaw, AActor* SeenTarget = nullptr);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* EnemyCapsule;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UEnemyGunComponent* EnemyGunComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* EnemyIndicatorWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bCanEnemyBeKilled = true;	
	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bShouldAvoidBullet = true;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		float LifeSpanAfterDeath = 10.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		float TimeToStartShooting = 1.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		float ShootTime = 1.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		FFloatRange StartShootingRandomTimeRange = FFloatRange(1.f, 3.f);

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
		TArray<FHitBoneType> HitBoneTypes;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Custom Hit on Bone")
		USoundBase* DefaultBoneHitSound;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Enemy Run Away")
		bool bCanEnemyRunAway = true;
	// if enemy has less hp then MaxEnemyHealthForRunAway then choose random if enemy should run away
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float MaxEnemyHealthForRunAway = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float SpeedOfEnemyWhenIsRunningAway = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float ChanceOfEnemyToRunAway = 20.f;

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
		
	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* FootstepsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		USoundBase* FootstepsRunningAwaySound;

	//Materials
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* ShotBloodDecalMaterial;

	//Particles
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		FColor BloodColor;
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UParticleSystem* EnemyBloodParticle;

	//Got Hit
	bool bIsDead;
	void ShouldRunAway();

	// Shooting
	FTimerHandle StartShootingHandle;
	FTimerHandle ShootHandle;
	void StartShooting();
	void Shoot();

	// Effects
	void SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType);
	void SpawnShotBloodDecal(const FHitResult& Hit);
	void SpawnBloodOnObjectDecal(const AActor* BulletThatHitEnemy, const FVector& HitLocation);

	// Damage
	bool bIsRunningAway;
	bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius);
	bool EnemyRunAway();

	//If enemy see the player then he will execute given functions
	AActor* FocusedActor;
	bool bEnemyDetectedTarget;

	// Enemy Indicator Widget
	class UEnemyIndicatorWidget* EnemyIndicatorWidget;

	//Footsteps sound
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	//EnemyAIController
	void SetEnemyKilledInAIController();
	void SetEnemyRunawayInAIController();

	int32 CurrentUniqueID = 0;
	void RemoveEnemySavedDataFromSave();
};
