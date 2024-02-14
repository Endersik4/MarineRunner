// Copyright Adam Bartela.All Rights Reserved

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

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetCanMove(bool bCan);

	void CallAlbertosToThePlayer(FVector PlayerLoc);
private:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBehaviorTree* AIBehaviour;
	
	bool bIsMovingTowardsPlayer;
	class AAlbertosPawn* AlbertosOwner;
};
