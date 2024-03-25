// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/Tasks/BTTask_UpdateCurrentLocationNumber.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SettingCurrentLocation::UBTTask_SettingCurrentLocation()
{
	NodeName = TEXT("SettingNumberOfCurrentLocations");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_SettingCurrentLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<UBlackboardComponent> BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!IsValid(BlackBoardComp))
		return EBTNodeResult::Failed;

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
