// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTServiceGoingToLocations.generated.h"

/**
 *  This service will pick random place between range (-2000,2000) from its current location.
 *  If current location is equel 0 then it will clear RandomLocation
 */
UCLASS()
class MARINERUNNER_API UBTServiceGoingToLocations : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTServiceGoingToLocations();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void PickRandomLocation(UBehaviorTreeComponent& OwnerComp);
	FVector GetRandomLocation();

	APawn* AIPawn;
	UBlackboardComponent* BlackBoardComp;
};
