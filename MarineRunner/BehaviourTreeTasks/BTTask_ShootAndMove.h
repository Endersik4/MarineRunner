// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ShootAndMove.generated.h"

/**
 * Enemy will be constanlty shooting in Time that Blackboard key says and
 * avoid bullets from player (with random location)
 */
UCLASS()
class MARINERUNNER_API UBTTask_ShootAndMove : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_ShootAndMove();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
private:
	void FinishTask(UBehaviorTreeComponent& OwnerComp);
	void MovedSuccessfully();

	void AvoidBullets();
	FTimerHandle AvoidHandle;

	void Shoot();
	FTimerHandle ShootHandle;

	class AEnemyPawn* EnemyPawn;
	class AEnemyAiController* EnemyAIController;
};
