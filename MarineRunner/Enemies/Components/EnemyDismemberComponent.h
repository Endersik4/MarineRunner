// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyDismemberComponent.generated.h"

USTRUCT(BlueprintType)
struct FDismemberLimb
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> LimbMesh = nullptr;
	UPROPERTY(EditAnywhere)
	FName BoneName = FName();

	const bool operator==(const FName& otherLimbName) const
	{
		return BoneName == otherLimbName;
	}

	const bool operator==(const FDismemberLimb& otherLimb) const
	{
		return BoneName == otherLimb.BoneName;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UEnemyDismemberComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyDismemberComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	void DismemberLimb(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, const FHitResult& HitBoneResult, float ImpulseForce, float RadialImpulseRadius = 0.f);
		
private:
	UPROPERTY(EditDefaultsOnly, Category = "Dismember System")
	TArray<FDismemberLimb> AllLimbsToDismember;

	void TerminateBone(TObjectPtr<USkeletalMeshComponent> SkeletalMeshToDismember, FName BoneNameToTerminate);
};
