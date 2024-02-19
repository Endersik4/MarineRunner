// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/GunClasses/BulletData.h"

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Shoot();

	UFUNCTION(BlueprintPure)
		const bool CanShootAgain();

	FVector PredictWhereToShoot(bool bIgnoreOffset = false);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 MagazineCapacity = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bFindBulletRotationTowardsTarget = true;
	// Rotation for bullet will be taken from bone 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TArray<FName> BulletSocketsNames = { TEXT("Bullet") };
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TArray<FName> MuzzleFleshSocketNames = { TEXT("MuzzleFlash") };
	// if == 0 then do not predict up bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_UP = 100.f;
	// if == 0 then do not predict forward bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_Forward = 100.f;
	// if == 0 then do not predict right bullet rotation
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootOffset_Right = 100.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Predict")
		float PredictWhereToShootDistanceDivider = 100.f;

	//  Bullet settings
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		FBulletStruct BulletData;
	// Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet")
		bool bManyBulletAtOnce;
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
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Bullet")
		FFloatRange BulletSpeedClamp = FFloatRange(700.f, 1000000.f);
	
	//Delay time after the enemy runs out of ammunition
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float DelayTimeMagazine;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Sounds")
		USoundBase* ShootingSound;

	bool bCanShoot = true;
	bool CanShoot();
	int32 CurrentSocketNameIndex = 0;
	void Reload();

	void ShootEffects();

	FTimerHandle ImpulseOnBulletHandle;
	void SpawnManyBullets();
	void SpawnBullet();
	FRotator CalculateBulletRotation();

	void AddImpulseDuringShooting();

	int32 InitialMagazineCapacity;
	bool bIsReloading;
	FTimerHandle DelayEmptyMagazineHandle;
	void DelayAfterEmptyMagazine();

	class IEnemyInterface* OwningEnemyInterface;
};
