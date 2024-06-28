// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletData.h"
#include "MarineRunner/Utilities/CustomDecalUtility.h"

#include "Bullet.generated.h"

UENUM()
enum ETypeOfObject {
	EnemyType,
	GlassType,
	ObjectType,
};

UCLASS()
class MARINERUNNER_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetBulletData(const FBulletStruct& NewBulletData) { BulletData = NewBulletData; }

private:
	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> BulletMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletLifeSpan = 15.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	float BulletFallingDown = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Trail")
	TObjectPtr<class UNiagaraSystem> BulletTrailNiagaraParticle = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TObjectPtr<UParticleSystem> BulletHitParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	FVector BulletHitParticleSize = FVector(1.f);
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	TObjectPtr<USoundBase> BulletHitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float BulletImpulseMultiplier = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hit")
	float BulletStuckInActorTeleportValue = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Hit Radial Impulse")
	TEnumAsByte<ECollisionChannel> RadialDamageCollisionChannel = ECC_GameTraceChannel3;
	UPROPERTY(EditDefaultsOnly, Category = "Hit Radial Impulse|Shake")
	float CameraShakeScaleMultiplier = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Hit Radial Impulse|Shake")
	float MaxDistanceToStartShake = 9000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hit Radial Impulse|Shake")
	float MaxCameraShakeScale = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
	FDecalBase BulletHoleDecal = FDecalBase(FVector(1.f), nullptr, 10.f, 2.f);

	void SetBulletMovementType();

	UPROPERTY(Transient)
	FBulletStruct BulletData = FBulletStruct();

	void SphereRadialDamage(const FHitResult& Hit);
	void BulletThroughObject(const FHitResult& Hit);

	//Bullet Movement
	UPROPERTY(Transient)
	bool bStartBulletMovement = false;
	UPROPERTY(Transient)
	float TrackBulletDistance = 0.f;
	UPROPERTY(Transient)
	FVector BulletLocation = FVector::Zero();
	void MovementBullet(float Delta);

	//Bullet Stack in the same actor after another move
	UPROPERTY(Transient)
	TObjectPtr<AActor> HitActor = nullptr;
	bool BulletStuckInActor(const FHitResult& Hit);

	void HitActorWithoutInterface(const FHitResult& HitResult);
	void UseDamageInterfaceOnActor(const FHitResult& HitResult);
	void StartCameraShake();

	void SpawnEffectsWhenHit(const FHitResult& Hit);

	UPROPERTY(Transient)
	TObjectPtr<class UNiagaraComponent> SpawnedBulletTrailNiagara = nullptr;
};
