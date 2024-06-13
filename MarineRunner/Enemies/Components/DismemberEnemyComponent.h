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
	// for blood spread
	UPROPERTY(EditAnywhere)
	FName UpperBoneName = FName();
	UPROPERTY(EditAnywhere)
	FRotator LimbRotation = FRotator(0.f);
	UPROPERTY(EditAnywhere)
	float LimbMassScale = 10.f;

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
		UpperBoneName = FName();
		LimbRotation = FRotator(0.f);
		LimbMassScale = 10.f;
	}

	FDismemberLimb(TObjectPtr<USkeletalMesh> _LimbSkeletalMesh, FName _BoneName, FName _UpperBoneName, FRotator _LimbRotation, float _LimbMassScale)
	{
		LimbSkeletalMesh = _LimbSkeletalMesh;
		BoneName = _BoneName;
		UpperBoneName = _UpperBoneName;
		LimbRotation = _LimbRotation;
		LimbMassScale = _LimbMassScale;
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
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float TimeToTerminateBone = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float LimbLifeSpan = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	float ImpulseForceMultiplier = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	TObjectPtr<class UNiagaraSystem> BloodSprayNiagaraSystem = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	FRotator BloodSprayAddRotation = FRotator(0.f, 0.f, -90.f);

	FDismemberLimb* SpawnLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, const float& ImpulseForce, const float& RadialImpulseRadius);
	void TerminateBone(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, FName BoneNameToTerminate);
	void SpawnBloodSprayParticle(FDismemberLimb* DismemberedLimb, TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FColor& BloodSprayColor);
};
