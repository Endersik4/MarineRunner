// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
		class UStaticMeshComponent* BulletMesh;

	void SetBulletVariables(float NewDamage, float NewAmmoSpeed, float NewAmmoDistance, float NewAmmoFallingDown, float NewAmmoImpulseForce);
	void SetBulletGoThroughVariables(bool bCanBulletGoThrough, float DamageReduceAfterObject, float ImpulseReduceAfterObject, int32 MaxObjectsForBulletToGoThrough);

	void ImpulseOnBullet();
private:
	UFUNCTION()
		void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* BulletHitParticle;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* BulletHit2Particle;

	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		bool bUseImpulseForBullet;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ObjectHitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* BulletHoleDecalMaterial;

	float Damage;
	float AmmoSpeed;
	float AmmoDistance;
	float AmmoFallingDown;
	float AmmoImpulseForce;

	bool bCanBulletGoThrough;
	float DamageReduceAfterObject;
	float ImpulseReduceAfterObject;
	int32 MaxObjectsForBulletToGoThrough;

	void MovementBullet(float Delta);
	bool bUseMyMovement;

	void SpawnEffectsForImpact(const FHitResult& Hit);
	void SpawnBulletHole(const FHitResult& Hit);
	void BulletThroughObject(const FHitResult& Hit);

	float TrackBulletDistance;
	FVector BulletLocation;
};
