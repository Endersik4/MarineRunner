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

private:
	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBehaviorTree* AIBehaviour;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;

	bool bDoEnemySeePlayer;
};
