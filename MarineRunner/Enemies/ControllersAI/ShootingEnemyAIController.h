// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShootingEnemyAIController.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API AShootingEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AShootingEnemyAIController();

	virtual void Tick(float DeltaTime) override;

	void EnemyKilled(bool bRunAwayInsteadOfKill = false);
	void AddEnemyToDetected(bool bWas);

private:
	UFUNCTION()
	void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UFUNCTION()
	void DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor, bool bStartAttackingTheTarget = true);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UAIPerceptionComponent> EnemyPerception = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	TObjectPtr<class UBehaviorTree> AIBehaviour = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	int32 HowManyLocations = 5;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup|Sight")
	TSubclassOf<class UAISense> SightSenseClass = nullptr;
	//How long does it take to detect the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup|Sight")
	float DetectPlayerTime = 0.2f;
	//How long does it take to lose sight of the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup|Sight")
	float LoseSightOfPlayerTime = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup|Hearing")
	TSubclassOf<class UAISense> HearingSenseClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup|Hearing")
	float StopDetectingPlayerTime = 70.f;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	float TimeToSetAIVariables = 0.05f;

	void SetAIVariables();
	UPROPERTY(Transient)
	bool bDead = false;
	UPROPERTY(Transient)
	bool bDoEnemySeePlayer = false;

	const bool CanSeeTheTarget(const TObjectPtr<AActor> TargetActor);

	FTimerHandle DetectPlayerDelayHandle;
	FTimerHandle PlayerNotHeardHandle;

	void SightHandle(TObjectPtr<AActor> SensedActor, const struct FAIStimulus& SightStimulus);
	void HearingHandle(TObjectPtr<AActor> SensedActor, const struct FAIStimulus& HearingStimulus);

	UPROPERTY(Transient)
	TObjectPtr<class AShootingEnemyPawn> OwningEnemyPawn = nullptr;
};
