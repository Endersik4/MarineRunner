// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"
#include "BrainComponent.h"

#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

AEnemyAiController::AEnemyAiController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));

}

void AEnemyAiController::BeginPlay()
{
	Super::BeginPlay();

	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAiController::HandleTargetPerceptionUpdated);

	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyAiController::OnMoveCompleted);
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAiController::KillEnemy()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
	bIsDead = true;
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
	if (IsValid(Actor) ==  false || IsValid(GetBlackboardComponent()) == false) return;
	if (!Actor->ActorHasTag("Player") || GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway")) || bIsDead) return;

	GetWorld()->GetTimerManager().ClearTimer(DetectPlayerDelayHandle);
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("DetectPlayerWithDelay"), Stimulus.WasSuccessfullySensed());
	GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, TimerDel, Stimulus.WasSuccessfullySensed() ? DetectPlayerTime : LoseSightOfPlayerTime, false);
}

void AEnemyAiController::DetectPlayerWithDelay(bool bIsDetected)
{
	bDoEnemySeePlayer = bIsDetected;

	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	AddEnemyToDetected(bIsDetected);
}

void AEnemyAiController::AddEnemyToDetected(bool bWas)
{
	if (bWas == true) MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn(), bIsDead);
	else MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
}

void AEnemyAiController::SetAIVariables()
{
	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

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
