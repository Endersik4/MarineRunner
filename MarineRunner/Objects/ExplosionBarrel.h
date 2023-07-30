// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/InteractInterface.h"

#include "ExplosionBarrel.generated.h"

UCLASS()
class MARINERUNNER_API AExplosionBarrel : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosionBarrel();

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius = 0.f) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ExplosionBarrelMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TSubclassOf<AActor> ExplosionBarrelGeometryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionDamage = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionImpulseForce = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionRadius = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		bool bDrawDebugRadialSphere;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		TSubclassOf<UCameraShakeBase> CameraShakeAfterExplosion;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Effects")
		USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Effects")
		UMaterialInstance* ExplosionDecal;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Effects")
		float ExplosionDecalSize = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Effects")
		class UParticleSystem* ExplosionParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Effects")
		float ExplosionParticleSize = 1.f;

	void Explode();
	void UseInterfaceOnActor(const FHitResult& HitResult);
	void SpawnEffects();

	bool bExploded;
};
