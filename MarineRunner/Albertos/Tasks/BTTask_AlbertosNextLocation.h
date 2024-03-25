// Copyright Adam Bartela.All Rights Reserved

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


public:
	UPROPERTY(EditAnywhere)
		FName PlayerLocationValueName = "PlayerLocation";
	UPROPERTY(EditAnywhere)
		float RadiusToPickFromPlayerLocation = 2000.f;
private:
	const FVector CalculateNextLocationNearThePlayer();
	
};
