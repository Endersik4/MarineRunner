// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Shoot.generated.h"

/**
 * This Task play ShootBullet function that is in EnemyPawn.cpp
 */
UCLASS()
class MARINERUNNER_API UBTTask_Shoot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Shoot();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
