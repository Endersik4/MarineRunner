// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/Tasks/BTTask_AlbertosNextLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "MarineRunner/Albertos/AlbertosAIController.h"


UBTTask_AlbertosNextLocation::UBTTask_AlbertosNextLocation()
{
	NodeName = TEXT("Albertos Next Location");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AlbertosNextLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<UBlackboardComponent> BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackBoardComp) 
		return EBTNodeResult::Failed;

	FVector NextLocation = CalculateNextLocationNearThePlayer();

	// if Player Location vector value is set then go to the Player Location instead of Caluclated Next Location
	if (BlackBoardComp->IsVectorValueSet(PlayerLocationValueName))
	{
		NextLocation = BlackBoardComp->GetValueAsVector(PlayerLocationValueName);
		BlackBoardComp->ClearValue(PlayerLocationValueName);
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NextLocation);

	return EBTNodeResult::Type();
}

FVector UBTTask_AlbertosNextLocation::CalculateNextLocationNearThePlayer()
{
	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		return FVector(0.f);

	FNavLocation RandomNavLocation;
	TObjectPtr<UNavigationSystemV1> NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (!IsValid(NavSystem))
		return FVector(0.f);

	NavSystem->GetRandomReachablePointInRadius(UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetActorLocation(), RadiusToPickFromPlayerLocation, RandomNavLocation);

	return RandomNavLocation.Location;
}
