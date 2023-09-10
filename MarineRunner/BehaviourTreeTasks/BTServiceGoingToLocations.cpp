// Fill out your copyright notice in the Description page of Project Settings.


#include "BTServiceGoingToLocations.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTServiceGoingToLocations::UBTServiceGoingToLocations()
{
	NodeName = TEXT("Set up Random Location");
	bNotifyBecomeRelevant = true;
	bCreateNodeInstance = true;
}

void UBTServiceGoingToLocations::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AIPawn = OwnerComp.GetAIOwner()->GetPawn();
	BlackBoardComp = OwnerComp.GetBlackboardComponent();

	if (BlackBoardComp) BlackBoardComp->ClearValue(TEXT("LastKnownPlayerLocation"));
}

void UBTServiceGoingToLocations::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	PickRandomLocation(OwnerComp);
}

void UBTServiceGoingToLocations::PickRandomLocation(UBehaviorTreeComponent& OwnerComp)
{
	if (AIPawn == nullptr || BlackBoardComp == nullptr) return;

	if (BlackBoardComp->GetValueAsInt(TEXT("CurrentLocations")) > 0) BlackBoardComp->SetValueAsVector(GetSelectedBlackboardKey(), GetRandomLocation());
	else BlackBoardComp->ClearValue(GetSelectedBlackboardKey());
}

FVector UBTServiceGoingToLocations::GetRandomLocation()
{
	float Rand_X = FMath::FRandRange(-2000.f, 2000.f) + AIPawn->GetActorLocation().X;
	float Rand_Y = FMath::FRandRange(-2000.f, 2000.f) + AIPawn->GetActorLocation().Y;
	float Rand_Z = AIPawn->GetActorLocation().Z;

	return FVector(Rand_X, Rand_Y, Rand_Z);
}
