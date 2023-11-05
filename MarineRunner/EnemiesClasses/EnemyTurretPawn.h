// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/EnemiesClasses/EnemyInterface.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE virtual USkeletalMeshComponent* GetSkeletalMesh() override { return TurretSkeletalMesh; }
	FORCEINLINE virtual const FVector GetPlayerCameraLocation() override { return FocusedActorLocation; }
	FORCEINLINE virtual void AddImpulseToPhysicsMesh(const FVector& Impulse) override {};
	FORCEINLINE virtual void PlayShootAnimation() override { PlayShootAnimMontage(); }
	FORCEINLINE virtual const bool GetIsDead() override { return false; }

	void PlayerWasSeen(bool bWas, AActor* ActorSeen);

	UFUNCTION(BlueprintImplementableEvent)
		void BoneDirectionToFocusedActor(const FRotator& RotationToActor, const FName& BoneToRotate);

	UFUNCTION(BlueprintImplementableEvent)
		void PlayShootAnimMontage();

	UFUNCTION(BlueprintPure)
		bool IsStillShooting();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* TurretSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UEnemyGunComponent* TurretGunComponent;

	UPROPERTY(EditAnywhere, Category = "Turret Settings")
		TArray<FRotateTurretBone> RotateTurretBones;
	UPROPERTY(EditAnywhere, Category = "Turret Settings")
		float TimeBetweenShoots = 0.1f;

	FTimerHandle StartShootingHandle;
	void Shoot();
	void PredictWhereToShoot(AActor* Actor);

	bool bRotateBones;
	AActor* FocusedActor;
	void LimitAngleAccordingToRange(double& Angle, const FFloatRange& Range);
	void RotateBonesTowardDetectedActor(float Delta);

	FVector FocusedActorLocation;
};
