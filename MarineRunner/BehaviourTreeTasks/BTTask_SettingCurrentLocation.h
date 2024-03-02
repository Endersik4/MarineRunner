// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SettingCurrentLocation.generated.h"

/**
 * This task tracks the number of current locations. If the enemy has successfully moved to a random location, the number will increase 
 * If the number of current locations is 0, the enemy returns to its starting position.
 */
UCLASS()
class MARINERUNNER_API UBTTask_SettingCurrentLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SettingCurrentLocation();
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
