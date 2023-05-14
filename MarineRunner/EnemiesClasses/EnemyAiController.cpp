// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"

AEnemyAiController::AEnemyAiController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));

	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAiController::HandleTargetPerceptionUpdated);
	
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyAiController::OnMoveCompleted);
}

void AEnemyAiController::BeginPlay()
{
	Super::BeginPlay();

	SetAIVariables();
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAiController::KillEnemy()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
}

void AEnemyAiController::RunAway()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("isRunningAway"), true);
}

void AEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	bIsMoveToCompleted = true;
}

void AEnemyAiController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor || !GetBlackboardComponent()) return;
	if (!Actor->ActorHasTag("Player") || GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway"))) return;

	FTimerDelegate TimerDel;

	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectPlayerDelayHandle);

		TimerDel.BindUFunction(this, FName("DetectPlayerWithDelay"), true);
		GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, TimerDel, DetectPlayerTime, false);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectPlayerDelayHandle);

		TimerDel.BindUFunction(this, FName("DetectPlayerWithDelay"), false);
		GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, TimerDel, LoseSightOfPlayerTime, false);
	}
	
}

void AEnemyAiController::DetectPlayerWithDelay(bool bIsDetected)
{
	bDoEnemySeePlayer = bIsDetected;
}

void AEnemyAiController::SetAIVariables()
{
	if (!AIBehaviour) return;
	RunBehaviorTree(AIBehaviour);

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());
	if (!EnemyPawn) return;

	DetectPlayerTime = EnemyPawn->GetDetectPlayerTime();
	LoseSightOfPlayerTime = EnemyPawn->GetLoseSightOfPlayerTime();

	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), EnemyPawn->GetHowManyLocations());
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), EnemyPawn->GetHowManyLocations());

	GetBlackboardComponent()->SetValueAsFloat(TEXT("WaitTime"), EnemyPawn->GetWaitTimeShoot());
}
