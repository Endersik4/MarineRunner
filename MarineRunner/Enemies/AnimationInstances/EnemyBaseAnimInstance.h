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
		float CurrentRotatedActorAngle;
	UPROPERTY(BlueprintReadOnly)
		float CurrentSpeed;

private:
	// divides current velocity 
	UPROPERTY(EditDefaultsOnly)
		float SpeedDivider = 3.5f;

	float CurrentVelocity;
	FVector PreviousLocation;

	UPROPERTY(Transient)
		TObjectPtr<APawn> PawnOwner;
};
