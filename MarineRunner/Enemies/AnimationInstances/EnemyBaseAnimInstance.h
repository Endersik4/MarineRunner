// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyBaseAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API UEnemyBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly)
	float CurrentRotatedActorAngle = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float CurrentSpeed = 0.f;

private:
	// divides current velocity 
	UPROPERTY(EditDefaultsOnly)
	float SpeedDivider = 3.5f;

	UPROPERTY(Transient)
	float CurrentVelocity = 0.f;
	UPROPERTY(Transient)
	FVector PreviousLocation = FVector::Zero();

	UPROPERTY(Transient)
	TObjectPtr<APawn> PawnOwner = nullptr;
};
