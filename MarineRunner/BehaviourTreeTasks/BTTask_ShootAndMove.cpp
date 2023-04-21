// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ShootAndMove.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "MarineRunner/EnemiesClasses/EnemyAiController.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"

UBTTask_ShootAndMove::UBTTask_ShootAndMove()
{
	NodeName = TEXT("Shoot And Move");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UBTTask_ShootAndMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	float Time = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(GetSelectedBlackboardKey());
	GetWorld()->GetTimerManager().SetTimer(ShootHandle, this, &UBTTask_ShootAndMove::Shoot, Time, true);

	EnemyPawn = Cast<AEnemyPawn>(OwnerComp.GetAIOwner()->GetPawn());
	EnemyAIController = Cast<AEnemyAiController>(OwnerComp.GetAIOwner());
	EnemyAIController->SetFocus(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	AvoidBullets();

	return EBTNodeResult::InProgress;
}

void UBTTask_ShootAndMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (EnemyAIController->GetDoEnemySeePlayer() == false)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
		GetWorld()->GetTimerManager().ClearTimer(AvoidHandle);

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

	if (EnemyAIController->GetIsMoveToCompleted())
	{
		UE_LOG(LogTemp, Warning, TEXT("MOVE COMPLETED SHOOT AND MOVe"));
		EnemyAIController->SetIsMoveToCompleted(false);
		GetWorld()->GetTimerManager().ClearTimer(AvoidHandle);
		AvoidBullets();
	}
}

void UBTTask_ShootAndMove::AvoidBullets()
{
	UE_LOG(LogTemp, Warning, TEXT("AVOID"));
	float Rand_X = FMath::FRandRange(-750, 750) + EnemyPawn->GetActorLocation().X;
	float Rand_Y = FMath::FRandRange(-750, 750) + EnemyPawn->GetActorLocation().Y;
	FVector Location = FVector(Rand_X, Rand_Y, EnemyPawn->GetActorLocation().Z);
	EnemyAIController->MoveToLocation(Location, 100.f);

	float RandomTime = FMath::FRandRange(1.5f, 4.f);
	GetWorld()->GetTimerManager().SetTimer(AvoidHandle, this, &UBTTask_ShootAndMove::AvoidBullets, RandomTime, false);
}

void UBTTask_ShootAndMove::Shoot()
{
	if (EnemyPawn == nullptr) return;

	EnemyPawn->ShootBullet();
}

