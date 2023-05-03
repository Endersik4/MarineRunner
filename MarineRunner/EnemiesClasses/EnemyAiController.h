// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAiController.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API AEnemyAiController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	AEnemyAiController();

	virtual void Tick(float DeltaTime) override;

	bool GetDoEnemySeePlayer() const { return bDoEnemySeePlayer; }
	bool GetIsMoveToCompleted() const { return bIsMoveToCompleted; }

	void SetIsMoveToCompleted(bool bNewMove) { bIsMoveToCompleted = bNewMove; }

	void KillEnemy();
	void RunAway();

private:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UFUNCTION()
		void DetectPlayerWithDelay(bool bIsDetected);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBehaviorTree* AIBehaviour;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;

	bool bDoEnemySeePlayer;
	bool bIsMoveToCompleted;

	FTimerHandle DetectPlayerDelayHandle;
	
	float DetectPlayerTime;
	float LoseSightOfPlayerTime;

	void SetAIVariables();
};
