// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/BehaviourTreeTasks/BTTask_GetRandomLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_GetRandomLocation::UBTTask_GetRandomLocation()
{
	NodeName = TEXT("Get Random Location");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_GetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	PickRandomLocation(OwnerComp);
	return EBTNodeResult::Succeeded;
}

void UBTTask_GetRandomLocation::SetOwner(AActor* ActorOwner)
{
	OwningActor = ActorOwner;
}

void UBTTask_GetRandomLocation::PickRandomLocation(UBehaviorTreeComponent& OwnerComp)
{
	if (IsValid(OwnerComp.GetBlackboardComponent()) == false)
		return;

	if (bClearValue == true)
	{
		if (OwnerComp.GetBlackboardComponent()->GetValueAsInt(TEXT("CurrentLocations")) <= 0)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), GetRandomLocation());
			return;
		}
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), GetRandomLocation());

}

FVector UBTTask_GetRandomLocation::GetRandomLocation()
{
	if (IsValid(OwningActor) == false)
		return FVector(0.f);

	FNavLocation RandomNavLocation;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (IsValid(NavSystem) == false)
		return FVector(0.f);

	NavSystem->GetRandomReachablePointInRadius(OwningActor->GetActorLocation(), RandomLocationRadius, RandomNavLocation);

	return RandomNavLocation.Location;
}