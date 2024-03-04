// Copyright Adam Bartela.All Rights Reserved

#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BrainComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/EnemiesClasses/TypesOfEnemy/ShootingEnemyPawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

AEnemyAiController::AEnemyAiController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));
}

void AEnemyAiController::BeginPlay()
{
	Super::BeginPlay();

	EnemyPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AEnemyAiController::HandleTargetPerceptionUpdated);

	SetAIVariables();
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region /////////// PERCEPTION ///////////////
void AEnemyAiController::HandleTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == SightSenseClass)
		SightHandle(TargetActor, Stimulus);
	else if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == HearingSenseClass)
		HearingHandle(TargetActor, Stimulus);
}

void AEnemyAiController::SightHandle(TObjectPtr<AActor> SensedActor, const FAIStimulus& SightStimulus)
{
	if (!CanSeeTheTarget(SensedActor))
		return;

	if (SightStimulus.WasSuccessfullySensed() && GetWorld()->GetTimerManager().IsTimerActive(DetectPlayerDelayHandle))
		return;

	FTimerDelegate DetectPlayerWithDelayDelegate;
	DetectPlayerWithDelayDelegate.BindUFunction(this, FName("DetectPlayerWithDelay"), SightStimulus.WasSuccessfullySensed(), SensedActor);
	GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, DetectPlayerWithDelayDelegate, SightStimulus.WasSuccessfullySensed() ? DetectPlayerTime : LoseSightOfPlayerTime, false);
}

void AEnemyAiController::HearingHandle(TObjectPtr<AActor> SensedActor, const FAIStimulus& HearingStimulus)
{
	if (!IsValid(SensedActor) || !HearingStimulus.WasSuccessfullySensed() || bDoEnemySeePlayer)
		return;

	FRotator RotatePawnTowardsSound = GetPawn()->GetActorRotation();
	RotatePawnTowardsSound.Yaw = (UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), SensedActor->GetActorLocation())).Yaw;
	GetPawn()->SetActorRotation(RotatePawnTowardsSound);
	
	if (!bDoEnemySeePlayer)
		StopMovement();

	DetectPlayerWithDelay(true, SensedActor, false);
}

void AEnemyAiController::DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor, bool bStartAttackingTheTarget)
{
	bDoEnemySeePlayer = bIsDetected;

	if (bDoEnemySeePlayer)
	{
		SetFocus(DetectedActor);
		GetBlackboardComponent()->SetValueAsObject(TEXT("FocusedActor"), DetectedActor);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		GetBlackboardComponent()->ClearValue(TEXT("FocusedActor"));

		if (IsValid(DetectedActor))
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), DetectedActor->GetActorLocation());
	}

	AddEnemyToDetected(bDoEnemySeePlayer);

	TObjectPtr<class AShootingEnemyPawn > EnemyPawn = Cast<AShootingEnemyPawn>(GetPawn());
	if (!IsValid(EnemyPawn)) 
		return;
	EnemyPawn->SawTheTarget(bDoEnemySeePlayer, DetectedActor, bStartAttackingTheTarget);
}

bool AEnemyAiController::CanSeeTheTarget(TObjectPtr<AActor> TargetActor)
{
	if (!IsValid(TargetActor) || !IsValid(GetBlackboardComponent())) 
		return false;
	if (!TargetActor->ActorHasTag("Player")|| GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway"))) 
		return false;

	return true;
}
#pragma endregion

void AEnemyAiController::AddEnemyToDetected(bool bWas)
{
	TObjectPtr<UMarineRunnerGameInstance> MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (!IsValid(MarineRunnerGameInstance))
		return;

	if (bWas && !bDead)
	{
		MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn());
	}
	else
	{
		MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
	}
}

void AEnemyAiController::SetAIVariables()
{
	if (!AIBehaviour) 
		return;
	RunBehaviorTree(AIBehaviour);

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), HowManyLocations);
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);

	TObjectPtr<AShootingEnemyPawn > EnemyPawn = Cast<AShootingEnemyPawn>(GetPawn());
	if (!IsValid(EnemyPawn)) 
		return;

	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());
}

void AEnemyAiController::EnemyKilled(bool bRunAwayInsteadOfKill)
{
	bDead = true;

	EnemyPerception->SetSenseEnabled(SightSenseClass, false);
	EnemyPerception->SetSenseEnabled(HearingSenseClass, false);
	ClearFocus(EAIFocusPriority::Gameplay);

	AddEnemyToDetected(false);
	GetWorld()->GetTimerManager().ClearTimer(DetectPlayerDelayHandle);

	if (bRunAwayInsteadOfKill)
	{
		GetBlackboardComponent()->SetValueAsBool(TEXT("isRunningAway"), true);
		return;
	}

	GetBrainComponent()->StopLogic(FString("dead"));
}
