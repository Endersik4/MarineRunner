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

	bool GetIsSliding() const { return bShouldSlide; }
	bool GetIsCrouching() const { return bIsCrouching; }

	void SetShouldSlide(bool bShould) { bShouldSlide = bShould; }
	void BeginSlide();
private:

	//Speed of croaching
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CrouchForceSpeed = 40000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float SpeedOfCrouchLerp = 10.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		FFloatRange ScalePlayerWhenCrouching = FFloatRange(1.5f, 2.5f);
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CrouchPressedVignetteIntensity = 0.7f;

	//How fast Velocity will be subtracted from Initial Velocity Of Sliding (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideSpeed = 200000.f;
	//Initial Velocity of Sliding is added to Walking Distance from MarineCharacter
	//and then we have Initial Velocity of Sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float InitialVelocityOfSliding = 115000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float MaxSlideForce = 120000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideDelayInSeconds = 0.08f;
	//How fast Pawn will gain speed on ramp when sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		float RampForce = 10000.f;
	//How fast Velocity will be subtracted from Initial Velocity Of Sliding on ramp (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		float SlideSpeedRamp = 450000.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide|Ramp")
		TSubclassOf<UCameraShakeBase> RampCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* SlideSound;

	float CurrentMovementForce;

	float InitialMovementForce;
	bool bIsCrouching;
	bool bCrouchPressed;

	bool SweepBox(FVector Where, float Distance);
	bool bShouldStillCroach = false;

	// Slide on ramp
	void SlideOnRamp(const float & Delta);

	// Camera Shake while sliding on ramp
	bool bStartRampCameraShake = false;
	UCameraShakeBase* CameraShakeBase;

	// Delay for start sliding
	FTimerHandle SlideDelayHandle;
	void Sliding(float Delta);
	bool ShouldStopSliding();
	bool bShouldSlide;

	bool bShouldPlaySound = true;
	class UAudioComponent* SpawnedSlideSound;
	void TurnOffSlideSound();

	float VignetteIntensityValue = 1.1f;

	void CroachLerp(float Delta);
	bool bCanCroachLerp;
	float ScaleZ = 1.5f;
		
	APlayerController* PlayerController;
	class AMarineCharacter* MarinePawn;

};

