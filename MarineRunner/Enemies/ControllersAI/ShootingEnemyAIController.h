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
	TSubclassOf<class UAISense> SightSenseClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	TSubclassOf<class UAISense> HearingSenseClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	int32 HowManyLocations = 5;
	//How long does it take to detect the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	float DetectPlayerTime = 0.2f;
	//How long does it take to lose sight of the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
	float LoseSightOfPlayerTime = 5.f;

	void SetAIVariables();
	UPROPERTY(Transient)
	bool bDead;
	UPROPERTY(Transient)
	bool bDoEnemySeePlayer;

	const bool CanSeeTheTarget(const TObjectPtr<AActor> TargetActor);

	FTimerHandle DetectPlayerDelayHandle;

	void SightHandle(TObjectPtr<AActor> SensedActor, const struct FAIStimulus& SightStimulus);
	void HearingHandle(TObjectPtr<AActor> SensedActor, const struct FAIStimulus& HearingStimulus);

};
