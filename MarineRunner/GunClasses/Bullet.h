// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
		class UStaticMeshComponent* BulletMesh;

	void SetBulletData(const FBulletStruct& NewBulletData) { BulletData = NewBulletData; }

private:
	UFUNCTION()
		void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Particles")
		FVector BulletHitParticleSize = FVector(1.f);
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* BulletHitParticle;
	UPROPERTY(EditAnywhere, Category = "Particles")
		class UParticleSystem* BulletHit2Particle;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ObjectHitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Decals")
		UMaterialInstance* BulletHoleDecalMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Decals")
		FVector BulletHoleDecalSize = FVector(1.f);
	UPROPERTY(EditDefaultsOnly, Category = "Decals")
		float BulletHoleFadeOutStartDelay = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Decals")
		float BulletHoleFadeOutDuration = 5.f;

	void SetBulletMovementType();

	FBulletStruct BulletData;

	void SphereRadialDamage(const FHitResult& Hit);	
	void BulletThroughObject(const FHitResult& Hit);

	//Bullet Movement
	bool bUseMyMovement;
	float TrackBulletDistance;
	FVector BulletLocation;
	void MovementBullet(float Delta);

	//Bullet Stack in the same actor after another move
	AActor* HitActor;
	bool BulletStuckInActor(const FHitResult& Hit);

	//Hit
	void HitActorWithoutInterface(const FHitResult& HitResult);
	void UseInterfaceOnActor(const FHitResult& HitResult);

	//Effects
	void SpawnEffectsWhenHit(const FHitResult& Hit);
	void SpawnBulletHoleDecal(const FHitResult& Hit);

};
