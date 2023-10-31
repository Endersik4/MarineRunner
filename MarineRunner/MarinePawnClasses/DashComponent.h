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
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Dash();
	bool GetIsPerformingDash() const { return bIsPerformingDash; }
private:
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashDistance = 2500.f;
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashTime = 0.2f;
	//Dash cooldown in Seconds
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashCoolDown = 1.8f;
	// Offset from obstacle that is on the dash way 
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float OffsetFromObstacle = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Settings")
		TSubclassOf<class UUserWidget> DashWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Sounds")
		USoundBase* DashSound;

	bool CanPlayerPerformDash() const;

	bool bCanDash = true;
	bool bIsPerformingDash;

	float DashTimeElapsed = 0.f;
	float CalculatedDashTime;

	FVector InitialPlayerPosition;
	FVector DashLocation;
	void LerpToDashLocation(float Delta);
	FVector CalculateEndDashPosition();
	const FVector CalculateDashDirection();
	bool GetCloserHitResult(FHitResult& OutHitResult);

	void TurnOffDash();
	void DashEffects();

	FTimerHandle DashCooldownHandle;
	FORCEINLINE void EndDashCooldown() { bCanDash = true; }

	class AMarineCharacter* MarinePawn;

};
