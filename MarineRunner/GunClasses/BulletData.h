// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BulletData.generated.h"

USTRUCT(BlueprintType)
struct FBulletStruct
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float Damage;
	// How fast ammo is moving forward. If Bullet has physics then this variable is Impulse Force
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float Speed;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		bool bUsePhysicsForMovement;
	// What distance should Ammo pass when bullet starts falling down
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (EditCondition = "!bUsePhysicsForMovement", EditConditionHides))
		float Distance;
	// How fast Ammo will falling down when AmmoDistance hit the number
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (EditCondition = "!bUsePhysicsForMovement", EditConditionHides))
		float FallingDown;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float HitImpulseForce;

	// Spawn Sphere in hit location, objects that are in sphere will take damage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		bool bUseSphereForDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (EditCondition = "bUseSphereForDamage", EditConditionHides))
		float RadialSphereRadius;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (EditCondition = "bUseSphereForDamage", EditConditionHides))
		bool bDrawDebugRadialSphere;

	// Can bullet go through objects that has material with Physics Asset = PM_BulletThrough
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		bool bCanBulletGoThroughObjects;
	// How much damage should be reduced after one object in percentage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects",
		meta = (EditCondition = "bCanBulletGoThroughObjects", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float DamageReduceAfterObject;
	// How much impulse should be reduced after one object in percentage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects",
		meta = (EditCondition = "bCanBulletGoThroughObjects", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float ImpulseReduceAfterObject;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects", meta = (EditCondition = "bCanBulletGoThroughObjects", EditConditionHides))
		int32 MaxObjectsForBulletToGoThrough;

	// When a bullet hits an object, if the player is near the bullet, the camera shake effect will be played.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (InlineEditConditionToggle))
		bool bShouldCameraShakeAfterHit;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet", meta = (EditCondition = "bShouldCameraShakeAfterHit"))
		TSubclassOf<UCameraShakeBase> CameraShakeAfterBulletHit;

	//Bullet Type that will be fired from Gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Objects to Spawn")
		TSubclassOf<class ABullet> BulletClass;
};


class MARINERUNNER_API BulletData
{
public:

};
