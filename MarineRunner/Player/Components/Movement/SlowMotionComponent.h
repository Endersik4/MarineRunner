// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SlowMotionComponent.generated.h"

/// <summary>
/// A Component that allow MarineCharacter to do Slow Motion with effect From DashWidget but without FOV 
/// In Details Panel you can set up variables for Slow Motion (speed,time,delay)
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API USlowMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USlowMotionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnOwnerDestroyed(AActor* DestroyedActor);
public:	

	void TurnOnSlowMotion() { SlowMotionPressed(); };
	FORCEINLINE bool GetIsInSlowMotion() const { return bIsInSlowMotion; }
	FORCEINLINE float GetCounterForceMultiplierWhenInAir() const {return CounterForceMultiplierWhenInAir;}

	void PauseSlowMotionSound(bool bPause);
private:
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionValue = 0.3f;
	//How long should Slow Motion be on
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionTime = 5.f;
	//Delay of Slow Motion before the next one
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionDelay = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float CounterForceMultiplierWhenInAir = 0.7f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float GlobalPitchModulation = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float StartingChromaticAbberation = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		FLinearColor ScreenColorWhenInSlowMotion = FLinearColor(FVector4(0.05f, 1.f, 0.24f,1.f));
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		TObjectPtr<USoundBase> SlowMotionSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		TObjectPtr<USoundBase> CancelSlowMotionSound = nullptr;

	UPROPERTY(Transient)
		bool bCanSlowMotion = true;
	UPROPERTY(Transient)
		bool bIsInSlowMotion = false;
	FTimerHandle SlowMotionDelayHandle;
	FTimerHandle SlowMotionTimeHandle;
	void SettingUpSlowMotion();

	void SlowMotionPressed();
	void SuddentDisabledSlowMotion();
	void DisableSlowMotion();

	void SlowMotionEffects();

	void SetCanSlowMotionAgain();
	
	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class UAudioComponent> SlowMotionSoundSpawned = nullptr;
};
