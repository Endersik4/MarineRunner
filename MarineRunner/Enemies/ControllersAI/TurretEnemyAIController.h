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

	void AddEnemyToDetected(bool bWas);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Turret AI Settings")
		float StopSeeingTheActorTime = 15.f;

	FTimerHandle StopSeeingTheActorHandle;

	UPROPERTY(Transient)
		bool bActorWasSeen = false;
	void StopSeeingActor();

	UPROPERTY(Transient)
		TObjectPtr<class AEnemyTurretPawn> TurretPawn = nullptr;

};
