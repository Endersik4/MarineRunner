// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/AI/TurretEnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/EnemiesClasses/EnemyTurretPawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

ATurretEnemyAIController::ATurretEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));

}

void ATurretEnemyAIController::BeginPlay()
{
	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretEnemyAIController::HandleTargetPerceptionUpdated);

	TurretPawn = Cast<AEnemyTurretPawn>(GetPawn());
	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

void ATurretEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	IInteractInterface* ActorCanTakeDamageInterface = Cast<IInteractInterface>(Actor);

	if (ActorCanTakeDamageInterface == nullptr || IsValid(TurretPawn) == false)
		return;

	if (Stimulus.WasSuccessfullySensed() == true)
	{
		GetWorldTimerManager().ClearTimer(StopSeeingTheActorHandle);
	}

	TurretPawn->PlayerWasSeen(Stimulus.WasSuccessfullySensed(), Actor);

	if (bActorWasSeen == true && Stimulus.WasSuccessfullySensed() == false && GetWorldTimerManager().IsTimerActive(StopSeeingTheActorHandle) == false)
	{
		GetWorldTimerManager().SetTimer(StopSeeingTheActorHandle, this, &ATurretEnemyAIController::StopSeeingActor, StopSeeingTheActorTime, false);
		return;
	}

	bActorWasSeen = Stimulus.WasSuccessfullySensed();
	AddEnemyToDetected(Stimulus.WasSuccessfullySensed());
}

void ATurretEnemyAIController::AddEnemyToDetected(bool bWas)
{
	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	if (bWas == true) MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn(), false);
	else MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
}

void ATurretEnemyAIController::StopSeeingActor()
{
	AddEnemyToDetected(false);
	bActorWasSeen = false;
}
