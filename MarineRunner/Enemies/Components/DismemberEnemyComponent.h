// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DismemberEnemyComponent.generated.h"

USTRUCT(BlueprintType)
struct FDismemberLimb
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> LimbSkeletalMesh = nullptr;
	UPROPERTY(EditAnywhere)
	FName BoneName = FName();
	UPROPERTY(EditAnywhere)
	FRotator LimbRotation = FRotator(0.f);

	const bool operator==(const FName& otherLimbName) const
	{
		return BoneName == otherLimbName;
	}

	const bool operator==(const FDismemberLimb& otherLimb) const
	{
		return BoneName == otherLimb.BoneName;
	}

	FDismemberLimb()
	{
		LimbSkeletalMesh = nullptr;
		BoneName = FName();
		LimbRotation = FRotator(0.f);
	}

	FDismemberLimb(TObjectPtr<USkeletalMesh> _LimbSkeletalMesh, FName _BoneName, FRotator _LimbRotation)
	{
		LimbSkeletalMesh = _LimbSkeletalMesh;
		BoneName = _BoneName;
		LimbRotation = _LimbRotation;
	}
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UDismemberEnemyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDismemberEnemyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void DismemberLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, float ImpulseForce, const FColor& BloodSprayColor, float RadialImpulseRadius = 0.f);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	TArray<FDismemberLimb> AllLimbsToDismember;
	// When a new limb spawns, the original bone is hidden, after which the bone is terminated.
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float TimeToTerminateBone = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float LimbLifeSpan = 30.f;
	// multiplier for /radial/ impulse to limb
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float ImpulseForceMultiplier = 5.f;
	// This value is added to LimbLifeSpawn when a new limb is spawned for a new enemy life spawn
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float AdditionalLifeSpanForEnemyOwner = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	TObjectPtr<class UNiagaraSystem> BloodSprayFromLimbNiagaraSystem = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	FRotator BloodSprayAddRotation = FRotator(0.f, 0.f, -90.f);

	// returns DismemberLimb from AllLimbsToDismember if found
	FDismemberLimb* SpawnLimbActor(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, const float& ImpulseForce, const float& RadialImpulseRadius);
	void TerminateBone(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, FName BoneNameToTerminate);
	void SpawnBloodSprayParticle(FDismemberLimb* DismemberedLimb, TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FColor& BloodSprayColor);

	UPROPERTY(Transient)
	TArray<FName> AllTerminatedBones;
};
