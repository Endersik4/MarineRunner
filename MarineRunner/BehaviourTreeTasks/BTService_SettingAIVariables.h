// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SettingAIVariables.generated.h"

/**
 *  This service will check if AIController see the player (thanks to AIPerception).
 *  If AIController See the player then variable PlayerLocation is set.
 *  Otherwise it will clear the value.
 */
UCLASS()
class MARINERUNNER_API UBTService_SettingAIVariables : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SettingAIVariables();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	void SetPlayerLocation(UBehaviorTreeComponent& OwnerComp);
	
};
