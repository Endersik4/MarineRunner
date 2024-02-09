// Copyright Adam Bartela.All Rights Reserved


#include "EnemyAiController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TimerManager.h"
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

	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AEnemyAiController::OnMoveCompleted);
}

void AEnemyAiController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyAiController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	bIsMoveToCompleted = true;
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

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
		return;
	EnemyPawn->SawTheTarget(bIsDetected, DetectedActor);

	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	AddEnemyToDetected(bIsDetected);
}

bool AEnemyAiController::bCanSeeTheTarget(AActor* TargetActor)
{
	if (IsValid(TargetActor) == false || IsValid(GetBlackboardComponent()) == false) 
		return false;
	if (TargetActor->ActorHasTag("Player") == false || GetBlackboardComponent()->GetValueAsBool(TEXT("isRunningAway")) || bIsDead) 
		return false;

	return true;
}
#pragma endregion

void AEnemyAiController::AddEnemyToDetected(bool bWas)
{
	if (IsValid(MarineRunnerGameInstance) == false)
		return;

	if (bWas == true) MarineRunnerGameInstance->AddNewDetectedEnemy(GetPawn(), bIsDead);
	else MarineRunnerGameInstance->RemoveDetectedEnemy(GetPawn());
}

void AEnemyAiController::SetAIVariables()
{
	MarineRunnerGameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (AIBehaviour == nullptr) 
		return;
	RunBehaviorTree(AIBehaviour);

	AEnemyPawn* EnemyPawn = Cast<AEnemyPawn>(GetPawn());
	if (IsValid(EnemyPawn) == false) 
		return;

	GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), EnemyPawn->GetActorLocation());

	GetBlackboardComponent()->SetValueAsInt(TEXT("HowManyLocations"), HowManyLocations);
	GetBlackboardComponent()->SetValueAsInt(TEXT("CurrentLocations"), HowManyLocations);

	GetBlackboardComponent()->SetValueAsFloat(TEXT("WaitTime"), EnemyPawn->GetWaitTimeShoot());
}

void AEnemyAiController::KillEnemy()
{
	GetBrainComponent()->StopLogic(FString("dead"));
}

void AEnemyAiController::RunAway()
{
	GetBlackboardComponent()->SetValueAsBool(TEXT("isRunningAway"), true);
}
