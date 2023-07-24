// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AlbertosAIController.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API AAlbertosAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	AAlbertosAIController();

	virtual void Tick(float DeltaTime) override;

	void SetCanMove(bool bCan);

	void CallAlbertosToThePlayer(FVector PlayerLoc);
private:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBehaviorTree* AIBehaviour;
	
	bool bIsMovingTowardsPlayer;
};
