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

	EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAiController::HandleTargetPerceptionUpdated);

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

void AEnemyAiController::SightHandle(AActor* SensedActor, const FAIStimulus& SightStimulus)
{
	if (bCanSeeTheTarget(SensedActor) == false)
		return;

	FTimerDelegate DetectPlayerWithDelayDelegate;
	DetectPlayerWithDelayDelegate.BindUFunction(this, FName("DetectPlayerWithDelay"), SightStimulus.WasSuccessfullySensed(), SensedActor);
	GetWorld()->GetTimerManager().SetTimer(DetectPlayerDelayHandle, DetectPlayerWithDelayDelegate, SightStimulus.WasSuccessfullySensed() ? DetectPlayerTime : LoseSightOfPlayerTime, false);
}

void AEnemyAiController::HearingHandle(AActor* SensedActor, const FAIStimulus& HearingStimulus)
{
	if (IsValid(SensedActor) == false || HearingStimulus.WasSuccessfullySensed() == false || bDoEnemySeePlayer == true)
		return;

	FRotator CurrentPawnRotation = GetPawn()->GetActorRotation();
	CurrentPawnRotation.Yaw = (UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), SensedActor->GetActorLocation())).Yaw;
	GetPawn()->SetActorRotation(CurrentPawnRotation);
	
	if (bDoEnemySeePlayer == false)
		StopMovement();

	DetectPlayerWithDelay(true, SensedActor, false);
}

void AEnemyAiController::DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor, bool bStartAttackingTheTarget)
{
	bDoEnemySeePlayer = bIsDetected;

	if (bDoEnemySeePlayer == true)
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

	AShootingEnemyPawn* EnemyPawn = Cast<AShootingEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
		return;
	EnemyPawn->SawTheTarget(bDoEnemySeePlayer, DetectedActor, bStartAttackingTheTarget);
}

bool AEnemyAiController::bCanSeeTheTarget(AActor* TargetActor)
{
	if (IsValid(TargetActor) == false || IsValid(GetBlackboardComponent()) == false) 
		return false;
	if (TargetActor->ActorHasTag("Player") == false || GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway"))) 
		return false;

	return true;
}
#pragma endregion

void AEnemyAiController::AddEnemyToDetected(bool bWas)
{
	UMarineRunnerGameInstance* MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	if (bWas == true && bDead == false)
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
	if (AIBehaviour == nullptr) 
		return;
	RunBehaviorTree(AIBehaviour);

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), HowManyLocations);
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);

	AShootingEnemyPawn* EnemyPawn = Cast<AShootingEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
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

	if (bRunAwayInsteadOfKill == true)
		return;

	GetBrainComponent()->StopLogic(FString("dead"));
}

void AEnemyAiController::RunAway()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("isRunningAway"), true);
}
