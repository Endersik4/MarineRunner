// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Enemies/ControllersAI/ShootingEnemyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BrainComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Enemies/TypesOfEnemy/ShootingEnemyPawn.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

AShootingEnemyAIController::AShootingEnemyAIController()
{
	EnemyPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIEnemyPerception"));
}

void AShootingEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	EnemyPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AShootingEnemyAIController::HandleTargetPerceptionUpdated);

	FTimerHandle SetAIVariablesHandle;
	GetWorld()->GetTimerManager().SetTimer(SetAIVariablesHandle, this, &AShootingEnemyAIController::SetAIVariables, TimeToSetAIVariables, false);
}

void AShootingEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region /////////// PERCEPTION ///////////////
void AShootingEnemyAIController::HandleTargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == SightSenseClass)
	{
		SightHandle(TargetActor, Stimulus);
	}
	else if (UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus) == HearingSenseClass)
	{
		HearingHandle(TargetActor, Stimulus);
	}
}

void AShootingEnemyAIController::SightHandle(TObjectPtr<AActor> SensedActor, const FAIStimulus& SightStimulus)
{
	if (!CanSeeTheTarget(SensedActor))
		return;

	FTimerDelegate DetectPlayerWithDelayDelegate;
	DetectPlayerWithDelayDelegate.BindUFunction(this, FName("DetectPlayerWithDelay"), SightStimulus.WasSuccessfullySensed(), SensedActor);
	GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, DetectPlayerWithDelayDelegate, SightStimulus.WasSuccessfullySensed() ? DetectPlayerTime : LoseSightOfPlayerTime, false);
}

void AShootingEnemyAIController::HearingHandle(TObjectPtr<AActor> SensedActor, const FAIStimulus& HearingStimulus)
{
	if (!IsValid(SensedActor) || !HearingStimulus.WasSuccessfullySensed() || bDoEnemySeePlayer)
		return;

	FRotator RotatePawnTowardsSound = GetPawn()->GetActorRotation();
	RotatePawnTowardsSound.Yaw = (UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), SensedActor->GetActorLocation())).Yaw;
	GetPawn()->SetActorRotation(RotatePawnTowardsSound);
	
	if (!bDoEnemySeePlayer)
		StopMovement();

	DetectPlayerWithDelay(true, SensedActor, false);

	FTimerDelegate PlayerNotHeardDelegate;
	PlayerNotHeardDelegate.BindUFunction(this, FName("DetectPlayerWithDelay"), false, nullptr);
	GetWorld()->GetTimerManager().SetTimer(PlayerNotHeardHandle, PlayerNotHeardDelegate, StopDetectingPlayerTime, false);
}

void AShootingEnemyAIController::DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor, bool bStartAttackingTheTarget)
{
	bDoEnemySeePlayer = bIsDetected;

	if (bDoEnemySeePlayer)
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerNotHeardHandle);
		if (IsValid(DetectedActor))
		{
			SetFocus(DetectedActor);
			if (IsValid(GetBlackboardComponent()))
				GetBlackboardComponent()->SetValueAsObject(TEXT("FocusedActor"), DetectedActor);
		}
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);

		if (IsValid(GetBlackboardComponent()))
			GetBlackboardComponent()->ClearValue(TEXT("FocusedActor"));

		if (IsValid(DetectedActor))
			GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), DetectedActor->GetActorLocation());
	}

	AddEnemyToDetected(bDoEnemySeePlayer);

	if (!IsValid(OwningEnemyPawn)) 
		return;

	OwningEnemyPawn->SawTheTarget(bDoEnemySeePlayer, DetectedActor, bStartAttackingTheTarget);
}

const bool AShootingEnemyAIController::CanSeeTheTarget(const TObjectPtr<AActor> TargetActor)
{
	if (!IsValid(TargetActor) || !IsValid(GetBlackboardComponent())) 
		return false;
	if (!TargetActor->ActorHasTag("Player")|| GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway"))) 
		return false;

	return true;
}
#pragma endregion

void AShootingEnemyAIController::AddEnemyToDetected(bool bWas)
{
	if (!IsValid(UGameplayStatics::GetGameInstance(GetWorld())))
		return;

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

void AShootingEnemyAIController::SetAIVariables()
{
	if (!AIBehaviour) 
		return;

	RunBehaviorTree(AIBehaviour);

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), HowManyLocations);
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);

	OwningEnemyPawn = Cast<AShootingEnemyPawn>(GetPawn());

	if (!IsValid(OwningEnemyPawn)) 
		return;

	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), OwningEnemyPawn->GetActorLocation());
}

void AShootingEnemyAIController::EnemyKilled(bool bRunAwayInsteadOfKill)
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
