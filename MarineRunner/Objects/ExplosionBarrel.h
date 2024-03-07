// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "ExplosionBarrel.generated.h"

UCLASS()
class MARINERUNNER_API AExplosionBarrel : public AActor, public IInteractInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosionBarrel();

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius = 0.f) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> ExplosionBarrelMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TSubclassOf<AActor> ExplosionBarrelGeometryClass;

	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float StartExplodeTime = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionDamage = 1200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionImpulseForce = 40000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		TObjectPtr<USoundBase>  ExplosionSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		float MaxDistanceToStartShake = 9000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		float CameraShakeScaleMultiplier = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		TSubclassOf<UCameraShakeBase> CameraShakeAfterExplosion;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		bool bDrawDebugRadialSphere;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Decal")
		TObjectPtr<UMaterialInstance>  ExplosionDecal;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Decal")
		float ExplosionDecalSize = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Particle")
		TObjectPtr<class UParticleSystem> ExplosionParticle;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Particle")
		float ExplosionParticleSize = 3.f;

	bool bExploded;
	void Explode();

	void UseDamageInterfaceOnActor(const FHitResult& HitResult);

	void SpawnEffects();
	void SpawnExplosionBarrelGeometry();
	TObjectPtr<AActor> SpawnedBarrelGeometry;

	int32 CurrentUniqueID = 0;
	void BarrelExplodedSaveData();
	void DisableBarrel(bool bDisable = true);

};
