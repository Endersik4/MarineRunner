// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/GunClasses/BulletData.h"

#include "EnemyShootComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UEnemyShootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyShootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
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
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Bullet", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		TArray<float>PitchBulletRecoilArray = { -5, 5 };
	// A random number is taken from this range and then added to the bullet Yaw rotation
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
		
};