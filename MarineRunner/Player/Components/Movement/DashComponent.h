// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDashComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Dash();
	FORCEINLINE bool GetIsPerformingDash() const { return bIsPerformingDash; }
private:
	UPROPERTY(EditAnywhere, Category = "Dash")
		float DashDistance = 2500.f;
	UPROPERTY(EditAnywhere, Category = "Dash")
		float DashTime = 0.2f;
	//Dash cooldown in Seconds
	UPROPERTY(EditAnywhere, Category = "Dash")
		float DashCoolDown = 1.8f;
	// Offset from obstacle that is on the dash way 
	UPROPERTY(EditAnywhere, Category = "Dash")
		float OffsetFromObstacle = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
		TSubclassOf<class UDashWidget> DashWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
		TObjectPtr<USoundBase> DashSound = nullptr;

	bool CanPlayerPerformDash() const;
	UPROPERTY(Transient)
		bool bCanDash = true;
	UPROPERTY(Transient)
		bool bIsPerformingDash = false;

	UPROPERTY(Transient)
		FVector InitialPlayerPosition = FVector::Zero();
	UPROPERTY(Transient)
		FVector DashLocation = FVector::Zero();

	/// <summary>
	/// Calculates closer hit result from two raycast, one from head location, the second from ground location
	/// </summary>
	/// <param name="OutHitResult"> Closer Hit Result</param>
	/// <returns>True if object was hit by one of the raycasts, false otherwise</returns>
	bool GetCloserHitResult(FHitResult& OutHitResult);
	FVector CalculateEndDashPosition();
	const FVector CalculateDashDirection();

	UPROPERTY(Transient)
		float DashTimeElapsed = 0.f;
	UPROPERTY(Transient)
		float CalculatedDashTime = 0.f;
	void MoveToDashLocation(float Delta);

	void TurnOffDash();
	void DashEffects();

	FTimerHandle DashCooldownHandle;
	FORCEINLINE void EndDashCooldown() { bCanDash = true; }

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn = nullptr;
};
