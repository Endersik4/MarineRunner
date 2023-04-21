// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

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

	if (AIBehaviour != nullptr)
	{
		
		RunBehaviorTree(AIBehaviour);

		AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());

		if (EnemyPawn)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());

			GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), EnemyPawn->GetHowManyLocations());
			GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), EnemyPawn->GetHowManyLocations());

			GetBlackboardComponent()->SetValueAsFloat(TEXT("WaitTime"), EnemyPawn->GetWaitTimeShoot());
		}
	}
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyAiController::KillEnemy()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("IsDead"), true);
}

void AEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	UE_LOG(LogTemp, Error, TEXT("KOMIEC"));
	bIsMoveToCompleted = true;
}

void AEnemyAiController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	bDoEnemySeePlayer = Stimulus.WasSuccessfullySensed();
}
