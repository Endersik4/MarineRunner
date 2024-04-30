// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/ControllersAI/TurretEnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/DamageInterface.h"
#include "MarineRunner/Enemies/TypesOfEnemy/EnemyTurretPawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

ATurretEnemyAIController::ATurretEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));

}

void ATurretEnemyAIController::BeginPlay()
{
	EnemyPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ATurretEnemyAIController::HandleTargetPerceptionUpdated);

	if (ensureMsgf(IsValid(GetPawn()), TEXT("Enemy Turret Pawn is nullptr in TurretEnemyAIController")))
	{
		TurretPawn = Cast<AEnemyTurretPawn>(GetPawn());
	}
}

void ATurretEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!IsValid(Actor) || !IsValid(TurretPawn))
		return;

	IDamageInterface* ActorCanTakeDamageInterface = Cast<IDamageInterface>(Actor);

	if (!ActorCanTakeDamageInterface)
		return;

	if (Stimulus.WasSuccessfullySensed())
	{
		GetWorldTimerManager().ClearTimer(StopSeeingTheActorHandle);
	}

	TurretPawn->PlayerWasSeen(Stimulus.WasSuccessfullySensed(), Actor);

	if (bActorWasSeen && !Stimulus.WasSuccessfullySensed() && !GetWorldTimerManager().IsTimerActive(StopSeeingTheActorHandle))
	{
		GetWorldTimerManager().SetTimer(StopSeeingTheActorHandle, this, &ATurretEnemyAIController::StopSeeingActor, StopSeeingTheActorTime, false);
		return;
	}

	bActorWasSeen = Stimulus.WasSuccessfullySensed();
	AddEnemyToDetected(Stimulus.WasSuccessfullySensed());
}

void ATurretEnemyAIController::AddEnemyToDetected(bool bWas)
{
	if (!IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

	TObjectPtr<UMarineRunnerGameInstance> MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(MarineRunnerGameInstance))
		return;

	if (bWas) 
		MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn());
	else
		MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
}

void ATurretEnemyAIController::StopSeeingActor()
{
	AddEnemyToDetected(false);
	bActorWasSeen = false;
}
