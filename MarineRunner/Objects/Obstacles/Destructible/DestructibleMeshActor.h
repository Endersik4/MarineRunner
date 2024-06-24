// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/DamageInterface.h"

#include "DestructibleMeshActor.generated.h"

struct FChaosPhysicsCollisionInfo;

UCLASS()
class MARINERUNNER_API ADestructibleMeshActor : public AActor, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADestructibleMeshActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, const EWeaponType& WeaponType, float NewSphereRadius = 0.f) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE void SetAssignedDestructibleActor(TObjectPtr<class ASpawnDestructibleActor> NewActor) { AssignedDestructibleActor = NewActor; }
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UGeometryCollectionComponent> DestructableMeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float ImpulseForceMultiplier = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float ExternalStrainStrength = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float ExternalStrainRadius = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float InternalStrainStrength = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float InternalStrainRadius = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	FFloatRange MaxMinRadialImpulseRadius = FFloatRange(100.f, 3000.f);
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings|Effects")
	TObjectPtr<USoundBase> DestructionHitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings|Effects")
	TObjectPtr<USoundBase> BreakSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings|Effects")
	float CanPlayBreakSoundTime = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings|Effects")
	TObjectPtr<UParticleSystem> DestructionHitParticle = nullptr;

	UFUNCTION()
	void OnChaosBreakEvent(const FChaosBreakEvent& BreakEvent);

	void DisableAssignedDestructibleActor();
	UPROPERTY(Transient)
	bool bVisible = false;
	UPROPERTY(Transient)
	bool bCanPlayBreakSound = true;
	FORCEINLINE void PlayAgainBreakSound() { bCanPlayBreakSound = true; }

	UPROPERTY(Transient)
	TObjectPtr<class ASpawnDestructibleActor> AssignedDestructibleActor = nullptr;
};
