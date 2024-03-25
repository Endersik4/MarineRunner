// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Gun/Bullet/BulletData.h"

#include "EnemyGunComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UEnemyGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyGunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	const bool CanShootAgain();
	void Shoot();

	const FVector PredictWhereToShoot(const bool bIgnoreOffset = false);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 MagazineCapacity = 10;
	//Delay time after the enemy runs out of ammunition
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float DelayTimeMagazine = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TObjectPtr<USoundBase> ShootingSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot Particle")
		TObjectPtr<UParticleSystem> ShootParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot Particle")
		float ShootParticleScale = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Predict")
		bool bFindBulletRotationTowardsTarget = true;
	// if == 0 then do not predict up bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_UP = 100.f;
	// if == 0 then do not predict forward bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_Forward = 100.f;
	// if == 0 then do not predict right bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_Right = 100.f;

	// Rotation for bullet will be taken from bone 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		TArray<FName> BulletSocketsNames = { TEXT("Bullet") };
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		TArray<FName> MuzzleFleshSocketNames = { TEXT("MuzzleFlash") };
	//  Bullet settings
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		FBulletStruct BulletData = FBulletStruct();
	// Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		bool bManyBulletAtOnce = false;
	// How many bullets spawn at once 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		int32 HowManyBulletsToSpawn = 10;
	// The impulse that is added to the opponent after the shot is fired
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		float RecoilImpulseOnEnemy = 3000.f;
	// A random number is taken from this range and then added to the bullet Pitch rotation
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		FFloatRange RandomRangeForPitchBullet = FFloatRange(-1.5f, 1.5f);
	// A random number is taken from this range and then added to the bullet Yaw rotation
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		FFloatRange RandomRangeForYawBullet = FFloatRange(-1.5f, 1.5f);

	UPROPERTY(Transient)
		bool bCanShoot = true;
	UPROPERTY(Transient)
		int32 CurrentSocketNameIndex = 0;
	bool CanShoot();
	void Reload();

	void ShootEffects();

	FTimerHandle ImpulseOnBulletHandle;
	void SpawnManyBullets();
	void SpawnBullet();
	const FRotator CalculateBulletRotation();

	void AddImpulseDuringShooting();

	UPROPERTY(Transient)
		int32 InitialMagazineCapacity = 0;
	UPROPERTY(Transient)
		bool bIsReloading = false;
	FTimerHandle DelayEmptyMagazineHandle;
	void DelayAfterEmptyMagazine();
	
	class IEnemyInterface* OwningEnemyInterface = nullptr;
};
