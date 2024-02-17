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

	EnemyOwner = Cast<APunchingEnemyPawn>(GetPawn());
}

void AChasingEnemyAIController::CheckDistanceToPlayer()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (IsValid(Player) == false || IsValid(GetPawn()) == false)
		return;

	if (FVector::Dist(GetPawn()->GetActorLocation(), Player->GetActorLocation()) > MaxDistanceToDetectPlayer)
	{
		EnemyOwner->PlayerDetected(false);
		return;
	}

	PlayerDetected(Player);
	EnemyOwner->PlayerDetected(true, Player);
	//GetWorld()->GetTimerManager().ClearTimer(DistanceCheckHandle);
}

void AChasingEnemyAIController::PlayerDetected(APawn* DetectedPlayer)
{
	MoveToActor(DetectedPlayer, 40.f);
}
