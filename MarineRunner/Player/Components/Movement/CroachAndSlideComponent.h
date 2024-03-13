// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CroachAndSlideComponent.generated.h"

DECLARE_DELEGATE_OneParam(FCrouchSlideDelegate, bool);
/// This component let you Croach and Slide
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
		float InitialVelocityOfSliding = 190000.f;
	//How fast Velocity will be subtracted from Initial Velocity Of Sliding (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideSpeed = 255000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float MaxSlideForce = 100000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideDelayInSeconds = 0.01f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		TObjectPtr<USoundBase> SlideSound;
	//How fast Pawn will gain speed on ramp when sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		float RampForce = 25000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		TSubclassOf<UCameraShakeBase> RampCameraShake;

	float CurrentMovementForce;
	float OriginalMovementForce;
	void SetPlayerVariables();

	bool bIsCrouching;
	bool bCrouchPressed;
	bool CanPlayerStand();
	void StopCrouching();
	FTimerHandle CheckCrouchReleasedHandle;

	// State of crouch (e.g standing/crouching)
	bool bChangingCrouchState;
	FVector OriginalPlayerScale;
	float CurrentVignetteIntensity;
	float CurrentActorScale_Z;
	float CrouchPressedTimeElapsed;
	float Target_VignetteIntensity = 1.1f;
	float Target_ScaleZ = 0.5f;
	void PrepareVariablesForChangingCrouchState(const float& TargetScale_Z, const float& TargetVignetteIntensity);
	void ChangingCrouchState(float Delta);

	// Slide on ramp
	void SlideOnRamp(const float & Delta);

	// Camera Shake while sliding on ramp
	bool bRampCameraShakeStarted = false;
	UPROPERTY(Transient)
		TObjectPtr<UCameraShakeBase> CameraShakeBase;

	// Delay for start sliding
	bool bSlide;
	FTimerHandle SlideDelayHandle;
	void Sliding(float Delta);
	bool ShouldStopSliding();
	void StopSliding();
	void StopSlideCameraShake();

	// Sliding sound
	bool bShouldPlaySound = true;
	void TurnOnSlideSound();
	void TurnOffSlideSound();
	UPROPERTY(Transient)
		TObjectPtr<class UAudioComponent> SpawnedSlideSound;

	UPROPERTY(Transient)
		TObjectPtr<APlayerController> PlayerController;
	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
};

