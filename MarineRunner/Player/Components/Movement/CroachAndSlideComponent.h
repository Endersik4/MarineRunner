// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CroachAndSlideComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCrouchSlideDelegate, bool);
/// This component let you Croach and Slide
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UCroachAndSlide : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCroachAndSlide();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOwnerDestroyed(AActor* DestroyedActor);
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CrouchPressed(bool bSlide = false);
	void CrouchReleased();
	void CrouchReleasedByObject();

	void BeginSlide();

	FORCEINLINE bool GetIsSliding() const { return bSlide; }
	FORCEINLINE	bool GetIsCrouching() const { return bIsCrouching; }

private:
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	float CrouchSpeed = 30000.f;
	// How fast change state of the crouch
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	float TimeToChangeCrouchState = 0.2f;
	// Actor Z Scale when crouching
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	float CrouchPlayerScale = 0.46f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	float CrouchPressedVignetteIntensity = 0.7f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	float CrouchReleasedTimeCheck = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
	FVector CheckBoxSizeToStandUpAfterCrouch = FVector(60.f, 60.f, 60.f);

	//Initial Velocity of Sliding is added to Walking Distance from MarineCharacter
	//and then we have Initial Velocity of Sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
	float InitialVelocityOfSliding = 130000.f;
	//How fast Velocity will be subtracted from Initial Velocity Of Sliding (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
	float SlideSpeed = 165000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
	float SlideDelayInSeconds = 0.01f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
	TObjectPtr<USoundBase> SlideSound = nullptr;
	//How fast Pawn will gain speed on ramp when sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
	float RampSlideDownForce = 25000.f;
	//How fast Pawn will lose speed on ramp when sliding up
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
	float RampSlideUpForce = 350000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
	float MaxSlideOnRampForce = 100000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
	TSubclassOf<UCameraShakeBase> RampCameraShake = nullptr;

	UPROPERTY(Transient)
	float CurrentMovementForce = 0.f;
	UPROPERTY(Transient)
	float OriginalMovementForce = 0.f;
	void SetPlayerVariables();

	UPROPERTY(Transient)
	bool bIsCrouching = false;
	UPROPERTY(Transient)
	bool bCrouchPressed = false;
	bool CanPlayerStand();
	void StopCrouching();
	FTimerHandle CheckCrouchReleasedHandle;

	// State of crouch (e.g standing/crouching)
	UPROPERTY(Transient)
	bool bChangingCrouchState = false;
	UPROPERTY(Transient)
	FVector OriginalPlayerScale = FVector::Zero();
	UPROPERTY(Transient)
	float CurrentVignetteIntensity = 0.f;
	UPROPERTY(Transient)
	float CurrentActorScale_Z = 0.f;
	UPROPERTY(Transient)
	float CrouchPressedTimeElapsed = 0.f;
	UPROPERTY(Transient)
	float Target_VignetteIntensity = 1.1f;
	UPROPERTY(Transient)
	float Target_ScaleZ = 0.5f;
	void PrepareVariablesForChangingCrouchState(const float& TargetScale_Z, const float& TargetVignetteIntensity);
	void ChangingCrouchState(float Delta);

	// Slide on ramp
	void SlideOnRamp(const float& Delta);

	// Camera Shake while sliding on ramp
	UPROPERTY(Transient)
	bool bRampCameraShakeStarted = false;
	UPROPERTY(Transient)
	TObjectPtr<UCameraShakeBase> CameraRampSlideShake = nullptr;

	// Delay for start sliding
	UPROPERTY(Transient)
	bool bSlide = false;
	FTimerHandle SlideDelayHandle;
	void Sliding(float Delta);
	bool ShouldStopSliding();
	void StopSliding();
	void StopSlideCameraShake();

	// Sliding sound
	UPROPERTY(Transient)
	bool bShouldPlaySound = true;
	void TurnOnSlideSound();
	void TurnOffSlideSound();
	UPROPERTY(Transient)
	TObjectPtr<class UAudioComponent> SpawnedSlideSound = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};

