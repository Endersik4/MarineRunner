// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceGoingToLocations.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTServiceGoingToLocations::UBTServiceGoingToLocations()
{
	NodeName = TEXT("Set up Random Location");
}

void UBTServiceGoingToLocations::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (AIPawn == nullptr || BlackBoardComp == nullptr) return;

	if (BlackBoardComp->GetValueAsInt(TEXT("CurrentLocations")) > 0)
	{
		float Rand_X = FMath::FRandRange(-2000, 2000) + AIPawn->GetActorLocation().X;
		float Rand_Y = FMath::FRandRange(-2000, 2000) + AIPawn->GetActorLocation().Y;
		float Rand_Z = AIPawn->GetActorLocation().Z;

		BlackBoardComp->SetValueAsVector(GetSelectedBlackboardKey(), FVector(Rand_X, Rand_Y, Rand_Z));
	}
	else
	{
		BlackBoardComp->ClearValue(GetSelectedBlackboardKey());
	}
}
