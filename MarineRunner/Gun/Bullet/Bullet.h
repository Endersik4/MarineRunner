// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletData.h"

#include "Bullet.generated.h"

UENUM()
enum TypeOfObject {
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
	UPROPERTY(EditDefaultsOnly, Category = "Hit|Shake")
		float CameraShakeScaleMultiplier = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Hit|Shake")
		float MaxDistanceToStartShake = 9000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hit|Shake")
		float MaxCameraShakeScale = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
		TObjectPtr<UMaterialInstance> BulletHoleDecalMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
		FVector BulletHoleDecalSize = FVector(1.f);
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
		float BulletHoleSize_X = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
		float BulletHoleFadeOutStartDelay = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Hole Decal")
		float BulletHoleFadeOutDuration = 5.f;

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

	//Hit
	void HitActorWithoutInterface(const FHitResult& HitResult);
	void UseDamageInterfaceOnActor(const FHitResult& HitResult);

	//Effects
	void SpawnEffectsWhenHit(const FHitResult& Hit);
	void SpawnBulletHoleDecal(const FHitResult& Hit);

	// Bullet Trail
	UPROPERTY(Transient)
		TObjectPtr<class UNiagaraComponent> SpawnedBulletTrailNiagara = nullptr;
};
