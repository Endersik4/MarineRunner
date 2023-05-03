// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CroachAndSlide.generated.h"

/// This component let you Croach and Slide

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UCroachAndSlide : public USceneComponent
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

	void CroachPressed();
	void CroachReleased();

	float GetCroachWalkingSpeed() const { return CroachForceSpeed; }
	bool GetIsSliding() const { return bShouldSlide; }

	void SetShouldSlide(bool bShould) { bShouldSlide = bShould; }
	void SetIsPressedForward(bool bShould) { bIsPressedForward = bShould; }
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerOfCroachForceWhenADS = 1.4f;
	//Speed of croaching
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float CroachForceSpeed = 1500.f;
	
	UPROPERTY(EditAnywhere, Category = "Movement|Croach")
		float SpeedOfCroachLerp = 10.f;
	//How fast Velocity will be subtracted from Initial Velocity Of Sliding (multiply by Delta Time)
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float SlideSpeed = 7.f;
	//Initial Velocity of Sliding is added to Walking Distance from MarineCharacter
	//and then we have Initial Velocity of Sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float InitialVelocityOfSliding = 7.f;
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float MaxSlideForce = 12000.f;
	//How fast Pawn will gain speed on ramp when sliding
	UPROPERTY(EditAnywhere, Category = "Movement|Slide")
		float RampForce = 5.f;
	class AMarineCharacter* MarinePawn;
	bool bIsPressedForward;

	float CopyForce;
	float Force;
	
	bool SweepBox(FVector Where, float Distance);
	bool bShouldStillCroach = false;

	void Sliding(float Delta);
	bool bShouldSlide;

	float VignetteIntensityValue = 1.2f;

	void CroachLerp(float Delta);
	bool bCanCroachLerp;
	float ScaleZ = 1.5f;
		
};
