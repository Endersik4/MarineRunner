// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/Components/AlbertosToPlayerComponent.h"

AAlbertosAIController::AAlbertosAIController()
{
}

void AAlbertosAIController::BeginPlay()
{
	Super::BeginPlay();

	AlbertosOwner = Cast<AAlbertosPawn>(GetPawn());;

	if (!AIBehaviour)
		return;
	RunBehaviorTree(AIBehaviour);
	SetCanMove(true);
}

void AAlbertosAIController::CallAlbertosToThePlayer(FVector PlayerLoc)
{
	GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PlayerLocation")), PlayerLoc);
	StopMovement();
	bIsMovingTowardsPlayer = true;
}

void AAlbertosAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!bIsMovingTowardsPlayer|| !IsValid(AlbertosOwner)) 
		return;

	AlbertosOwner->GetAlbertosToPlayerComponent()->ChangeMaxSpeedOfFloatingMovement(false);
	bIsMovingTowardsPlayer = false;
}

void AAlbertosAIController::SetCanMove(bool bCan)
{
	GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bCanMove")), bCan);
}
