// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CroachAndSlide.generated.h"

/// This component let you Croach and Slide
DECLARE_DELEGATE_OneParam(FCrouchSlideDelegate, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UCroachAndSlide : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCroachAndSlide();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CrouchPressed(bool bSlide = false);
	void CrouchReleased();

	void CrouchReleasedByObject();

	FORCEINLINE bool GetIsSliding() const { return bSlide; }
	FORCEINLINE	bool GetIsCrouching() const { return bIsCrouching; }

	void SetShouldSlide(bool bShould) { bSlide = bShould; }
	void BeginSlide();
private:

	//Speed of croaching
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CrouchForceSpeed = 30000.f;
	// How fast change state of the crouch
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float TimeToChangeCrouchState = 0.2f;
	// Lower Value is a target for Actor Z Scale when crouching, Upper value is for when standing up
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		FFloatRange ScalePlayerWhenCrouching = FFloatRange(0.4f, 1.f);
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CrouchPressedVignetteIntensity = 0.7f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CrouchReleasedTimeCheck = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		FVector CheckBoxSizeToStandUpAfterCrouch = FVector(60.f, 60.f, 60.f);

	//How fast Velocity will be subtracted from Initial Velocity Of Sliding (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideSpeed = 255000.f;
	//Initial Velocity of Sliding is added to Walking Distance from MarineCharacter
	//and then we have Initial Velocity of Sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float InitialVelocityOfSliding = 190000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float MaxSlideForce = 120000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideDelayInSeconds = 0.01f;
	//How fast Pawn will gain speed on ramp when sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		float RampForce = 70000.f;
	//How fast Velocity will be subtracted from Initial Velocity Of Sliding on ramp (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		float SlideSpeedRamp = 450000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		TSubclassOf<UCameraShakeBase> RampCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* SlideSound;

	float CurrentMovementForce;
	float OriginalMovementForce;

	bool bIsCrouching;
	bool bCrouchPressed;
	bool bCanPlayerStand();
	void StopCrouching();
	FTimerHandle CheckCrouchReleasedHandle;

	// State of crouch (e.g standing/crouching)
	bool bIsNowChangingCrouchState;
	float CurrentVignetteIntensity;
	float CurrentActorScale_Z;
	float CrouchPressedTimeElapsed;
	float Target_VignetteIntensity = 1.1f;
	float Target_ScaleZ = 0.5f;
	void PrepareVariablesForChangingCrouchState(const float& TargetScale_Z, const float& TargetVignetteIntensity);
	void StartChangingCrouchState(float Delta);

	// Slide on ramp
	void SlideOnRamp(const float & Delta);

	// Camera Shake while sliding on ramp
	bool bRampCameraShakeStarted = false;
	UCameraShakeBase* CameraShakeBase;

	// Delay for start sliding
	bool bSlide;
	FTimerHandle SlideDelayHandle;
	void Sliding(float Delta);
	bool ShouldStopSliding();
	void StopSliding();

	// Sliding sound
	void TurnOnSlideSound();
	bool bShouldPlaySound = true;
	class UAudioComponent* SpawnedSlideSound;
	void TurnOffSlideSound();

	APlayerController* PlayerController;
	class AMarineCharacter* MarinePawn;
};

