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

	if (OwnerComp.GetAIOwner()->GetPawn() && OwnerComp.GetBlackboardComponent()->GetValueAsInt(TEXT("CurrentLocations")) > 0)
	{
		float Rand_X = FMath::FRandRange(-2000, 2000) + OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation().X;
		float Rand_Y = FMath::FRandRange(-2000, 2000) + OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation().Y;
		float Rand_Z = OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation().Z;

		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), FVector(Rand_X, Rand_Y, Rand_Z));
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
	}
}
