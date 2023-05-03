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
	//Turn On TickTask()
	INIT_TASK_NODE_NOTIFY_FLAGS();
	bCreateNodeInstance = true;
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

EBTNodeResult::Type UBTTask_ShootAndMove::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	StopShootAndMove(OwnerComp.GetBlackboardComponent());
	return EBTNodeResult::Succeeded;
}

void UBTTask_ShootAndMove::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	FinishTask(OwnerComp);

	MovedSuccessfully();
}

//If Enemy doesnt see the player then stop shooting, avoiding and end task succesfully
void UBTTask_ShootAndMove::FinishTask(UBehaviorTreeComponent& OwnerComp)
{
	if (!EnemyAIController || !EnemyPawn) return;

	if (EnemyAIController->GetDoEnemySeePlayer() == false)
	{
		StopShootAndMove(OwnerComp.GetBlackboardComponent());

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

//IF enemy Completed MoveTo function then do another
void UBTTask_ShootAndMove::MovedSuccessfully()
{
	if (EnemyAIController == nullptr) return;

	if (EnemyAIController->GetIsMoveToCompleted())
	{
		EnemyAIController->SetIsMoveToCompleted(false);
		GetWorld()->GetTimerManager().ClearTimer(AvoidHandle);
		AvoidBullets();
	}
}

//Random Location where Enemy have to go. Its for "avoiding" bullets from player
void UBTTask_ShootAndMove::AvoidBullets()
{
	float Rand_X = FMath::FRandRange(-750.f, 750.f) + EnemyPawn->GetActorLocation().X;
	float Rand_Y = FMath::FRandRange(-750.f, 750.f) + EnemyPawn->GetActorLocation().Y;
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

void UBTTask_ShootAndMove::StopShootAndMove(UBlackboardComponent* BlackBoardComp)
{
	GetWorld()->GetTimerManager().ClearTimer(ShootHandle);
	GetWorld()->GetTimerManager().ClearTimer(AvoidHandle);

	EnemyAIController->ClearFocus(EAIFocusPriority::Gameplay);
	BlackBoardComp->ClearValue(TEXT("PlayerLocation"));

	if (EnemyPawn->GetIsReloading() == false) EnemyPawn->Reload();
	EnemyPawn->RestoreBonesToInitialRotation();
}

