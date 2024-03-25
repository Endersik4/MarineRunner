// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/AlbertosAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"

AAlbertosAIController::AAlbertosAIController()
{
}

void AAlbertosAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetPawn()), TEXT("Albertos Pawn is nullptr in Albertos AI Controller")))
	{
		AlbertosOwner = Cast<AAlbertosPawn>(GetPawn());
	}

	SetUpAIBehaviour();
}

void AAlbertosAIController::CallAlbertosToThePlayer(const FVector & PlayerLoc)
{
	GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PlayerLocation")), PlayerLoc);
	StopMovement();
	bIsMovingTowardsPlayer = true;
}

void AAlbertosAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (!bIsMovingTowardsPlayer|| !IsValid(AlbertosOwner)) 
		return;

	if (!IsValid(AlbertosOwner->GetAlbertosToPlayerComponent()))
		return;

	AlbertosOwner->GetAlbertosToPlayerComponent()->ChangeMaxSpeedOfFloatingMovement(false);
	bIsMovingTowardsPlayer = false;
}

void AAlbertosAIController::SetUpAIBehaviour()
{
	if (!AIBehaviour)
		return;

	RunBehaviorTree(AIBehaviour);
	SetCanMove(true);
}

void AAlbertosAIController::SetCanMove(bool bCan)
{
	if (!IsValid(GetBlackboardComponent()))
		return;

	GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bCanMove")), bCan);
}
