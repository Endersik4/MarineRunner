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
		void SetCanMove(bool bCan);

	void CallAlbertosToThePlayer(const FVector & PlayerLoc);
private:
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UBehaviorTree> AIBehaviour = nullptr;
	
	UPROPERTY(Transient)
		bool bIsMovingTowardsPlayer = false;
	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosOwner = nullptr;

	void SetUpAIBehaviour();
};
