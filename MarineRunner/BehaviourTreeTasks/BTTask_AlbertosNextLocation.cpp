// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/BehaviourTreeTasks/BTTask_AlbertosNextLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"


UBTTask_AlbertosNextLocation::UBTTask_AlbertosNextLocation()
{
	NodeName = TEXT("Albertos Next Location");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AlbertosNextLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackBoardComp) return EBTNodeResult::Failed;

	FVector NextLocation = CalculateNextLocationNearThePlayer();

	if (BlackBoardComp->IsVectorValueSet(FName(TEXT("PlayerLocation"))))
	{
		NextLocation = BlackBoardComp->GetValueAsVector(FName(TEXT("PlayerLocation")));
		BlackBoardComp->ClearValue(FName(TEXT("PlayerLocation")));
	}

	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NextLocation);

	return EBTNodeResult::Type();
}

FVector UBTTask_AlbertosNextLocation::CalculateNextLocationNearThePlayer()
{
	if (IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) == false)
		return FVector(0.f);

	FNavLocation RandomNavLocation;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (IsValid(NavSystem) == false)
		return FVector(0.f);

	NavSystem->GetRandomReachablePointInRadius(UGameplayStatics::GetPlayerPawn(GetWorld(),0)->GetActorLocation(), RadiusToPickFromPlayerLocation, RandomNavLocation);

	return RandomNavLocation.Location;
}
