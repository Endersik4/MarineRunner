// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ChasingEnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API AChasingEnemyAIController : public AAIController
{
	GENERATED_BODY()
protected:
	AChasingEnemyAIController();

	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Chasing Enemy AI Settings")
	float MaxDistanceToDetectPlayer = 5000.f;
	UPROPERTY(EditAnywhere, Category = "Chasing Enemy AI Settings")
	float CheckForDistanceInterval = 1.0f;

	FTimerHandle DistanceCheckHandle;
	void CheckDistanceToPlayer();

	UPROPERTY(Transient)
	bool bPlayerDetected = false;
};
