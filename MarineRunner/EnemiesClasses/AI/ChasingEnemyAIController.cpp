// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/AI/ChasingEnemyAIController.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/EnemiesClasses/TypesOfEnemy/PunchingEnemyPawn.h"

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
	TObjectPtr<APunchingEnemyPawn> EnemyOwner = Cast<APunchingEnemyPawn>(GetPawn());
	if (!IsValid(EnemyOwner))
		return;

	TObjectPtr<APawn> Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!IsValid(Player))
		return;

	if (FVector::Dist(EnemyOwner->GetActorLocation(), Player->GetActorLocation()) > MaxDistanceToDetectPlayer && !bPlayerDetected)
	{
		return;
	}

	bPlayerDetected = true;
	MoveToActor(Player, 40.f);
	EnemyOwner->PlayerDetected(true, Player);
}