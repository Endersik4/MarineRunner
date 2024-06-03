// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/ControllersAI/ChasingEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "MarineRunner/Enemies/TypesOfEnemy/PunchingEnemyPawn.h"

AChasingEnemyAIController::AChasingEnemyAIController()
{
}

void AChasingEnemyAIController::BeginPlay()
{
	Super::BeginPlay();


	GetWorld()->GetTimerManager().SetTimer(DistanceCheckHandle, this, &AChasingEnemyAIController::CheckDistanceToPlayer, CheckForDistanceInterval, true);
}

void AChasingEnemyAIController::CheckDistanceToPlayer()
{
	if (!ensureMsgf(IsValid(GetPawn()), TEXT("Punching Enemy Pawn is nullptr in ChasingEnemyAIController!")))
		return;

	TObjectPtr<APunchingEnemyPawn> EnemyOwner = Cast<APunchingEnemyPawn>(GetPawn());
	if (!IsValid(EnemyOwner))
		return;

	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		return;
	TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(Player))
		return;

	const float& DistanceToPlayer = FVector::Dist(EnemyOwner->GetActorLocation(), Player->GetActorLocation());
	if (DistanceToPlayer > MaxDistanceToDetectPlayer && !bPlayerDetected)
		return;

	bPlayerDetected = true;
	MoveToActor(Player, 40.f);
	EnemyOwner->PlayerDetected(true, Player);
}