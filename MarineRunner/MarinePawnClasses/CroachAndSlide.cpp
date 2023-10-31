// Fill out your copyright notice in the Description page of Project Settings.


#include "CroachAndSlide.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

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

	MovementForce = MarinePawn->GetMovementForce();

	CopyMovementForce = MovementForce;
	MarinePawn->GetCamera()->PostProcessSettings.bOverride_VignetteIntensity = true;
}

// Called every frame
void UCroachAndSlide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bShouldStillCroach == true)
	{
		CrouchReleased();
	}
	CroachLerp(DeltaTime);
	Sliding(DeltaTime);
}

void UCroachAndSlide::Sliding(float Delta)
{
	if (bShouldSlide == false) return;
 
	if (bShouldPlaySound == true && SlideSound)
	{
		SpawnedSlideSound = UGameplayStatics::SpawnSound2D(GetWorld(), SlideSound);
		bShouldPlaySound = false;
	}

	if (MarinePawn->GetIsOnRamp())
	{
		if (MarinePawn->GetIsGoingUp() == false)
		{
			MovementForce += (MovementForce < MaxSlideForce) ? (RampForce) * Delta : 0;
			if (bStartRampCameraShake == false)
			{
				CameraShakeBase = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(RampCameraShake, 1.f);
				bStartRampCameraShake = true;
			}
		}
		else
		{
			MovementForce -= SlideSpeedRamp * Delta;
		}
	}
	else
	{
		MovementForce -= SlideSpeed * Delta;
	}

	if (MovementForce <= CroachForceSpeed || (MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0))
	{
		MovementForce = CroachForceSpeed;
		TurnOffSlideSound();
		bShouldSlide = false;
	}

	MarinePawn->SetMovementForce(MovementForce);
}

void UCroachAndSlide::CrouchPressed(bool bSlide)
{
	if (MarinePawn->GetIsPlayerLerpingToHookLocation() || MarinePawn->GetIsWallrunning() || bIsCrouching == true) return;
	
	bIsCrouching = true;

	if (bShouldStillCroach) return;

	MovementForce = CroachForceSpeed;
	ScaleZ = 1.5f;
	VignetteIntensityValue = CrouchPressedVignetteIntensity;

	bCanCroachLerp = true;
	if (bSlide)
	{
		GetWorld()->GetTimerManager().SetTimer(SlideDelayHandle, this, &UCroachAndSlide::BeginSlide, SlideDelayInSeconds, false);
	}

	MarinePawn->SetMovementForce(MovementForce);
}

void UCroachAndSlide::BeginSlide()
{
	if (MarinePawn->GetIsGoingUp() == true)
		return;
	if (MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0)
		return;
	if (MarinePawn->GetIsJumping())
		return;

	MovementForce = CopyMovementForce + InitialVelocityOfSliding;
	bShouldSlide = true;
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

void UCroachAndSlide::CrouchReleased()
{
	if (MarinePawn->GetIsOnRamp()) 
		return;

	//Check if Pawn can stand
	if (SweepBox(GetOwner()->GetActorUpVector(), 100.f))
	{
		bShouldStillCroach = true;
		return;
	}
	bIsCrouching = false;
	bShouldPlaySound = true;

	bShouldStillCroach = false;
	bShouldSlide = false;
	TurnOffSlideSound();
	GetWorld()->GetTimerManager().ClearTimer(SlideDelayHandle);

	bStartRampCameraShake = false;
	if (IsValid(CameraShakeBase))
		CameraShakeBase->StopShake(false);

	ScaleZ = 2.5f;
	VignetteIntensityValue = 0.f;
	bCanCroachLerp = true;

	if (MarinePawn->GetIsInAir()) return;

	MovementForce = CopyMovementForce;
	MarinePawn->SetMovementForce(MovementForce);
}

bool UCroachAndSlide::SweepBox(FVector Where, float Distance)
{
	FVector Start = GetOwner()->GetActorLocation();
	Start.Z += 200.f;
	FVector End = Start + (Where * Distance);

	FHitResult HitResult;
	//DrawDebugBox(GetWorld(), Start, FVector(20, 20,60), FColor::Red, true);
	return GetWorld()->SweepSingleByChannel(HitResult, Start, Start, FQuat::Identity, ECC_GameTraceChannel6, FCollisionShape::MakeBox(FVector(30, 30, 60)));
}


void UCroachAndSlide::TurnOffSlideSound()
{
	if (!SpawnedSlideSound) return;

	SpawnedSlideSound->ToggleActive();
	SpawnedSlideSound = nullptr;
}