// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/AI/TurretEnemyAIController.h"
#include "Perception/AIPerceptionComponent.h"

#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/EnemiesClasses/EnemyTurretPawn.h"

ATurretEnemyAIController::ATurretEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("EnemyPerception"));

}

void ATurretEnemyAIController::BeginPlay()
{
	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ATurretEnemyAIController::HandleTargetPerceptionUpdated);

	TurretPawn = Cast<AEnemyTurretPawn>(GetPawn());
}

void ATurretEnemyAIController::HandleTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	IInteractInterface* ActorCanTakeDamageInterface = Cast<IInteractInterface>(Actor);

	if (ActorCanTakeDamageInterface == nullptr || IsValid(TurretPawn) == false)
		return;

	TurretPawn->PlayerWasSeen(Stimulus.WasSuccessfullySensed(), Actor);
}
