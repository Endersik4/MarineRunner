// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UBTTask_GetRandomLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_GetRandomLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void SetOwner(AActor* ActorOwner) override;

public:
	// Get random reachable point location where pawn location is origin and RandomLocationRadius is radius
	UPROPERTY(EditAnywhere)
		float RandomLocationRadius = 600.f;
private:

	void PickRandomLocation(UBehaviorTreeComponent& OwnerComp);
	FVector GetRandomLocation();

	AActor* OwningActor;
};
