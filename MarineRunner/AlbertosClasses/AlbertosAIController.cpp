// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"

AAlbertosAIController::AAlbertosAIController()
{
	
}

void AAlbertosAIController::BeginPlay()
{
	Super::BeginPlay();

	if (!AIBehaviour) return;
	RunBehaviorTree(AIBehaviour);
}

void AAlbertosAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAlbertosAIController::SetCanMove(bool bCan)
{
	GetBlackboardComponent()->SetValueAsBool(FName(TEXT("bCanMove")), bCan);
}

void AAlbertosAIController::CallAlbertosToThePlayer(FVector PlayerLoc)
{
	GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PlayerLocation")), PlayerLoc);
	StopMovement();
	bIsMovingTowardsPlayer = true;
}

void AAlbertosAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (bIsMovingTowardsPlayer == false) return;

	AAlbertosPawn* Albertos = Cast<AAlbertosPawn>(GetPawn());
	Albertos->ChangeMaxSpeedOfFloatingMovement(false);
	bIsMovingTowardsPlayer = false;
}
