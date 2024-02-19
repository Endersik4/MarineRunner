// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAiController.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API AEnemyAiController : public AAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	AEnemyAiController();

	virtual void Tick(float DeltaTime) override;

	void EnemyKilled(bool bRunAwayInsteadOfKill = false);
	void AddEnemyToDetected(bool bWas);

private:
	UFUNCTION()
		void HandleTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	UFUNCTION()
		void DetectPlayerWithDelay(bool bIsDetected, AActor* DetectedActor, bool bStartAttackingTheTarget = true);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UAIPerceptionComponent* EnemyPerception;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		class UBehaviorTree* AIBehaviour;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		TSubclassOf<class UAISense> SightSenseClass;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		TSubclassOf<class UAISense> HearingSenseClass;
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		int32 HowManyLocations = 5;
	//How long does it take to detect the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float DetectPlayerTime = 0.2f;
	//How long does it take to lose sight of the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float LoseSightOfPlayerTime = 5.f;

	void SetAIVariables();
	bool bDead;

	bool bDoEnemySeePlayer;
	bool bCanSeeTheTarget(AActor* TargetActor);

	FTimerHandle DetectPlayerDelayHandle;
	void SightHandle(AActor* SensedActor, const struct FAIStimulus& SightStimulus);
	void HearingHandle(AActor* SensedActor, const struct FAIStimulus& HearingStimulus);

};
