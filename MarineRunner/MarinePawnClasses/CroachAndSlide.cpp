// Fill out your copyright notice in the Description page of Project Settings.


#include "CroachAndSlide.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values for this component's properties
UCroachAndSlide::UCroachAndSlide()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCroachAndSlide::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	Force = MarinePawn->GetForce();

	CopyForce = Force;
	MarinePawn->GetCamera()->PostProcessSettings.bOverride_VignetteIntensity = true;
}

// Called every frame
void UCroachAndSlide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldStillCroach == true)
	{
		CroachReleased();
	}
	CroachLerp(DeltaTime);
	Sliding(DeltaTime);
}

void UCroachAndSlide::Sliding(float Delta)
{
	if (bShouldSlide == false) return;

	if (MarinePawn->GetIsOnRamp())
	{
		if (MarinePawn->GetIsGoingUp() == false) Force += (Force < MaxSlideForce) ? (RampForce) * Delta : 0;
		else Force -= (SlideSpeed * 2.5) * Delta;
		
	}
	else Force -= SlideSpeed * Delta;

	if (Force <= CroachForceSpeed)
	{
		Force = CroachForceSpeed;
		bShouldSlide = false;
	}

	MarinePawn->SetForce(Force);
}

void UCroachAndSlide::CroachPressed()
{
	Force = CroachForceSpeed;
	ScaleZ = 1.5f;
	VignetteIntensityValue = 1.2f;

	bCanCroachLerp = true;
	if (MarinePawn->GetIsGoingUp() == false)
	{
		if (MarinePawn->GetInputAxisValue(TEXT("Forward")) == 1.f || MarinePawn->GetInputAxisValue(TEXT("Right")) != 0)
		{
			if (MarinePawn->GetIsJumping()) return;
			Force = CopyForce + InitialVelocityOfSliding;
			bShouldSlide = true;
		}
	}

	MarinePawn->SetForce(Force);
}

void UCroachAndSlide::CroachLerp(float Delta)
{
	if (bCanCroachLerp == false) return;

	float NewVignetteIntensity = FMath::Lerp(MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity, VignetteIntensityValue, Delta * SpeedOfCroachLerp);
	MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity = NewVignetteIntensity;
	
	float NewScaleZ = FMath::Lerp(GetOwner()->GetActorScale3D().Z, ScaleZ, Delta * SpeedOfCroachLerp);
	GetOwner()->SetActorScale3D(FVector(2, 2, NewScaleZ));
	if (GetOwner()->GetActorScale3D().Equals(FVector(2, 2, ScaleZ), 0.01))
	{
		GetOwner()->SetActorScale3D(FVector(2, 2, ScaleZ));
		bCanCroachLerp = false;
	}
}

void UCroachAndSlide::CroachReleased()
{
	//Check if Pawn can stand
	if (SweepBox(GetOwner()->GetActorUpVector(), 100.f))
	{
		bShouldStillCroach = true;
		return;
	}
	bShouldSlide = false;
	bShouldStillCroach = false;

	ScaleZ = 2.5f;
	VignetteIntensityValue = 0.f;
	bCanCroachLerp = true;

	Force = CopyForce;
	MarinePawn->SetForce(Force);

	//GetOwner()->SetActorScale3D(FVector(2,2,2.5));
}

bool UCroachAndSlide::SweepBox(FVector Where, float Distance)
{
	FVector Start = GetOwner()->GetActorLocation();
	Start.Z += 200.f;
	FVector End = Start + (Where * Distance);

	FHitResult HitResult;
	//DrawDebugBox(GetWorld(), Start, FVector(20, 20,60), FColor::Red, true);
	return GetWorld()->SweepSingleByChannel(HitResult, Start, Start, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(FVector(30, 30, 60)));
}

