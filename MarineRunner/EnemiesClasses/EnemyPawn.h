// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/GunClasses/BulletData.h"

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
class MARINERUNNER_API AEnemyPawn : public APawn, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
 
	//Function From Interface
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius) override;

	//AI
	bool GetShouldAvoidBullets() const { return bShouldAvoidBullet; }
	bool GetPlayPrepareToShootAnimation() const { return bPlayPrepareToShootAnimation; }
	int32 GetHowManyLocations() const { return HowManyLocations; }
	float GetWaitTimeShoot() const { return WaitTimeShoot; }
	float GetDetectPlayerTime() const { return DetectPlayerTime; }
	float GetLoseSightOfPlayerTime() const { return LoseSightOfPlayerTime; }

	void ShootBullet() { Shoot(); }

	void Reload();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayPrepareToShootAnimation(bool bShouldPlay);

	UFUNCTION(BlueprintImplementableEvent)
		void SetShouldRunningAwayInAnimBP();

	UFUNCTION(BlueprintImplementableEvent)
		void PlayAnimMontageInBlueprint();

	UFUNCTION(BlueprintImplementableEvent)
		void RestoreBonesToInitialRotation();

	//Had to get AnimInstance of AnimationBlueprint so in blueprint you are setting what bone you want to looking at the player
	UFUNCTION(BlueprintImplementableEvent)
		void FocusBonesOnPlayerWhenPlayerDetected();

	// Bone Rotation - Bone will be looking at the player when he is detected.
	// bLookStraight means whether Bone should look directly at the player's camera or with some margin
	UFUNCTION(BlueprintCallable)
		FRotator FocusBoneOnPlayer(FName BoneName, bool bLookStraight);

	UFUNCTION(BlueprintCallable)
		void SetCanShoot(bool bCan) { bCanShoot = bCan; }

	UFUNCTION(BlueprintPure)
		int32 GetCurrentMagazineCapacity() const { return MagazineCapacity; }
	UFUNCTION(BlueprintPure)
		bool GetIsReloading() const { return bIsReloading; }
	UFUNCTION(BlueprintPure)
		bool GetIsDead() const { return bIsDead; }
	UFUNCTION(BlueprintPure)
		float GetSpeedOfEnemyWhenIsRunningAway() const { return SpeedOfEnemyWhenIsRunningAway; }

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* EnemySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* EnemyFloatingMovement;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* CapsuleColl;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		int32 HowManyLocations = 5;
	UPROPERTY(EditAnywhere, Category = "AI Setup")
		float WaitTimeShoot = 0.3f;
	//How long does it take to detect the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float DetectPlayerTime = 0.2f;
	//How long does it take to lose sight of the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float LoseSightOfPlayerTime = 5.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bCanEnemyRunAway = true;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float SpeedOfEnemyWhenIsRunningAway = 1600.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bShouldAvoidBullet = true;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bShouldAlsoPredictVertical;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy")
		bool bPlayPrepareToShootAnimation;

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
		
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		FBulletStruct BulletData;
	//Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		bool bManyBulletAtOnce;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		int32 HowManyBulletsToSpawn = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		float RecoilImpulseOnEnemy = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		TArray<float>PitchBulletRecoilArray = { -5, 5 };
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		TArray<float>YawBulletRecoilArray = { -5, 5 };;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 MagazineCapacity = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		FName BoneNameForBulletRotation = TEXT("Koncowka_Drugiego_Palca_R");

	//Delay time after the enemy runs out of ammunition
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float DelayTimeMagazine;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ShootingSound;
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

	//Shooting
	bool bCanShoot = true;
	void Shoot();
	void ShootEffects();
	void PredictWhereToShoot();
	void PushBackDuringShooting();

	FVector PlayerCameraLocation;

	//Got Hit
	bool bIsDead;
	void AlertEnemyAboutPlayer();
	void SetIsDead(bool bNewDead);
	void ShouldRunAway();

	// Effects
	void SpawnEffectsForImpact(const FHitResult& Hit, const FHitBoneType* PtrHitBoneType);
	void SpawnShotBloodDecal(const FHitResult& Hit);
	void SpawnBloodOnObjectDecal(const AActor* BulletThatHitEnemy, const FVector& HitLocation);

	// Damage
	bool bIsRunningAway;
	bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius);
	bool EnemyRunAway();

	//If enemy see the player then he will execute given functions
	void CheckIfEnemySeePlayer();
	bool ConditionsForEnemyToSeePlayer();

	//Reload
	bool bIsReloading;
	FTimerHandle DelayEmptyMagazineHandle;
	void DelayAfterEmptyMagazine();

	//Bullet
	FTimerHandle ImpulseOnBulletHandle;
	void SpawnManyBullets();
	void SpawnBullet();

	//Footsteps sound
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	//MarinePawn
	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();

	//EnemyAIController
	class AEnemyAiController* EnemyAIController;
	void SetUpEnemyAIController();

	//Other
	int32 CopyOfMagazineCapacity;
};
