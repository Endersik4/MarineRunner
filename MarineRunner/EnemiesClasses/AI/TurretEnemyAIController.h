// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TurretEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API ATurretEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ATurretEnemyAIController();

	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;
	
	class AEnemyTurretPawn* TurretPawn;
};
