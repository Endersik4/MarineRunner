// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/DamageInterface.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

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
		TObjectPtr<class UStaticMeshComponent> ExplosionBarrelMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TSubclassOf<AActor> ExplosionBarrelGeometryClass = nullptr;;

	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float StartExplodeTime = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionDamage = 1200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionImpulseForce = 40000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		float ExplosionRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		TObjectPtr<USoundBase>  ExplosionSound = nullptr;;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		float MaxDistanceToStartShake = 9000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		float CameraShakeScaleMultiplier = 0.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		TSubclassOf<UCameraShakeBase> CameraShakeAfterExplosion = nullptr;;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Camera Shake")
		float MaxCameraShakeScale = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel")
		bool bDrawDebugRadialSphere = false;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Decal")
		TObjectPtr<UMaterialInstance>  ExplosionDecal = nullptr;;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Decal")
		FVector ExplosionDecalSize = FVector(4.f);
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Particle")
		TObjectPtr<class UParticleSystem> ExplosionParticle = nullptr;;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Explosion Barrel|Explosion Particle")
		float ExplosionParticleSize = 3.f;

	UPROPERTY(Transient)
		bool bExploded = false;
	void Explode();

	void UseDamageInterfaceOnActor(const FHitResult& HitResult);

	void SpawnEffects();
	void SpawnExplosionBarrelGeometry();
	UPROPERTY(Transient)
		TObjectPtr<AActor> SpawnedBarrelGeometry;

	UPROPERTY(Transient)
		int32 CurrentUniqueID = 0;

	void BarrelExplodedSaveData();
	void DisableBarrel(bool bDisable = true);

};
