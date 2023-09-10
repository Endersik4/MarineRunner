// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AlbertosNextLocation.generated.h"

/**
 * Select a random location near the player to which Albertos will move
 */
UCLASS()
class MARINERUNNER_API UBTTask_AlbertosNextLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_AlbertosNextLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FVector CalculateNextLocationNearThePlayer(float Albertos_Z);
	
};
