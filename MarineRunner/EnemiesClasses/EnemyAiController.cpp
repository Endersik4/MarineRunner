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
		}
	}
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyAiController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	bDoEnemySeePlayer = Stimulus.WasSuccessfullySensed();
	UE_LOG(LogTemp, Warning, TEXT("DO ENEMY SEE %s"), (bDoEnemySeePlayer ? TEXT("TRUE") : TEXT("FALSE")));
}
