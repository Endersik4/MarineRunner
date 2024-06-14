// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/DamageInterface.h"
#include "DismemberedLimbActor.generated.h"

UCLASS()
class MARINERUNNER_API ADismemberedLimbActor : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
public:	
	ADismemberedLimbActor();

protected:
	virtual void BeginPlay() override;

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius = 0.f) override;


public:
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetLimbSkeletalMesh() const { return LimbSkeletalMesh; }

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> LimbSkeletalMesh = nullptr;
public:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UDismemberEnemyComponent> DismemberBodyComponent = nullptr;

};
