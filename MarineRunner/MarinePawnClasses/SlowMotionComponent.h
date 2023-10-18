// Fill out your copyright notice in the Description page of Project Settings.

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

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TurnOnSlowMotion() { SlowMotionPressed(); };
	bool GetIsInSlowMotion() const { return bIsInSlowMotion; }

private:
	//Speed of SlowMotion
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionValue = 0.4f;
	//How long should Slow Motion be on
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionTime = 4.7f;
	//Delay of Slow Motion before the next one
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float SlowMotionDelay = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		float StartingChromaticAbberation = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "SlowMotion Settings")
		FLinearColor ScreenColorWhenInSlowMotion = FLinearColor(FVector4(0.05f, 1.f, 0.24f,1.f));
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* SlowMotionSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* CancelSlowMotionSound;

	bool bCanSlowMotion = true;
	bool bIsInSlowMotion = false;
	FTimerHandle SlowMotionTimeHandle;
	FTimerHandle SlowMotionDelayHandle;
	void SettingUpSlowMotion();

	void SlowMotionPressed();
	void SuddentDisabledSlowMotion();
	void DisableSlowMotion();

	void SlowMotionEffects();

	void DelayCompleted();
	
	class AMarineCharacter* MarinePawn;
	class UAudioComponent* SlowMotionSoundSpawned;
};
