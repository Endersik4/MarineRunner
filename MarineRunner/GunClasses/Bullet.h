// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

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

	void ImpulseOnBullet();

private:
	UFUNCTION()
		void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UNiagaraComponent* BulletTrail;

	UPROPERTY(EditAnywhere, Category = "Particles")
		class UNiagaraSystem* BulletHits;

	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		bool bUseImpulseForBullet;
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		class UPaperSprite* BulletHoleSprite;

	float Damage;
	float AmmoSpeed;
	float AmmoDistance;
	float AmmoFallingDown;
	float AmmoImpulseForce;

	void MovementBullet(float Delta);
	bool bUseMyMovement;

	void DamageEnemy(class AEnemyPawn* Enemy, const FHitResult& Hit);
	void AlertEnemyAboutPlayer(class AEnemyPawn* Enemy);
	void SpawnBulletHole(const FHitResult& Hit);

	float TrackBulletDistance;
	FVector BulletLocation;
	UNiagaraComponent* HitParticle;
};
