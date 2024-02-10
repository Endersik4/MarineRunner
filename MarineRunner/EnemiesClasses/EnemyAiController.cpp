// Copyright Adam Bartela.All Rights Reserved


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "BrainComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
}

void AEnemyAiController::DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor)
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
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastKnownPlayerLocation"), DetectedActor->GetActorLocation());
		GetBlackboardComponent()->ClearValue(TEXT("FocusedActor"));
	}

	AddEnemyToDetected(bDoEnemySeePlayer);

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
		return;
	EnemyPawn->SawTheTarget(bDoEnemySeePlayer, DetectedActor);
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
	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	if (bWas == true)
	{
		MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn(), false);
	}
	else
	{
		MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
	}
}

void AEnemyAiController::SetAIVariables()
{
	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (AIBehaviour == nullptr) 
		return;
	RunBehaviorTree(AIBehaviour);

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), HowManyLocations);
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
		return;

	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());
}

void AEnemyAiController::EnemyKilled()
{
	EnemyPerception->SetSenseEnabled(SightSenseClass, false);
	EnemyPerception->SetSenseEnabled(HearingSenseClass, false);

	AddEnemyToDetected(false);
	GetWorld()->GetTimerManager().ClearTimer(DetectPlayerDelayHandle);

	GetBrainComponent()->StopLogic(FString("dead"));
}

void AEnemyAiController::RunAway()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("isRunningAway"), true);
}
