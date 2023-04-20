// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceGoingToLocations.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UBTServiceGoingToLocations : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTServiceGoingToLocations();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
