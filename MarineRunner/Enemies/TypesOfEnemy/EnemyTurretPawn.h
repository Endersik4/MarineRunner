// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Enemies/Interfaces/EnemyGunInterface.h"
#include "EnemyTurretPawn.generated.h"

USTRUCT(BlueprintType)
struct FRotateTurretBone
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	FName BoneName;
	UPROPERTY(EditAnywhere)
	FRotator RotateInAxis;
	UPROPERTY(EditAnywhere)
	bool bLimitedRotation;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bLimitedRotation", EditConditionHides))
	FFloatRange RotateAngleRange;

	FRotateTurretBone()
	{
		BoneName = "";
		RotateInAxis = FRotator(0.f);
		RotateAngleRange = FFloatRange(0.f);
		bLimitedRotation = false;
	}

	FRotateTurretBone(FName _BoneName, FRotator _RotateInAxis, FFloatRange _RotateAngleRange, bool _bLimitedRotation)
	{
		BoneName = _BoneName;
		RotateInAxis = _RotateInAxis;
		RotateAngleRange = _RotateAngleRange;
		bLimitedRotation = _bLimitedRotation;
	}
};

UCLASS()
class MARINERUNNER_API AEnemyTurretPawn : public APawn, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyTurretPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() override { return TurretSkeletalMesh; }
	virtual class AActor* GetFocusedActor() override { return FocusedActor; }
	FORCEINLINE virtual void AddImpulseToPhysicsMesh(const FVector& Impulse) override {};
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void PlayerWasSeen(bool bWas, AActor* ActorSeen);

	UFUNCTION(BlueprintImplementableEvent)
	void BoneDirectionToFocusedActor(const FRotator& RotationToActor, const FName& BoneToRotate);

	UFUNCTION(BlueprintPure)
	bool IsStillShooting();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> TurretSkeletalMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyGunComponent> TurretGunComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Turret Settings")
	TArray<FRotateTurretBone> RotateTurretBones = { FRotateTurretBone() };
	UPROPERTY(EditAnywhere, Category = "Turret Settings")
	float TimeBetweenShoots = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Turret Settings")
	TObjectPtr<UAnimMontage> ShootAnimMontage = nullptr;

	FTimerHandle StartShootingHandle;
	void Shoot();

	UPROPERTY(Transient)
	bool bRotateBones = false;
	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor;
	UPROPERTY(Transient)
	FVector FocusedActorLocation = FVector::Zero();

	void LimitAngleAccordingToRange(double& Angle, const FFloatRange& Range);
	void RotateBonesTowardDetectedActor(float Delta);
};
