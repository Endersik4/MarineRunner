// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SettingCurrentLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SettingCurrentLocation::UBTTask_SettingCurrentLocation()
{
	NodeName = TEXT("SettingNumberOfCurrentLocations");
	bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTTask_SettingCurrentLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackBoardComp)
	{
		return EBTNodeResult::Failed;
	}

	int32 CurrentLocations = BlackBoardComp->GetValueAsInt(TEXT("CurrentLocations"));
 	if (CurrentLocations <= 0)
	{
		int32 HowManyLocations = BlackBoardComp->GetValueAsInt(TEXT("HowManyLocations"));
		BlackBoardComp->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);
	}
	else
	{
		BlackBoardComp->SetValueAsInt(TEXT("CurrentLocations"), CurrentLocations - 1);
	}

	return EBTNodeResult::Succeeded;
}
