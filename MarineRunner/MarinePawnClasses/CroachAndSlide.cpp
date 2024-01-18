// Copyright Adam Bartela.All Rights Reserved

#include "CroachAndSlide.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"

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
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	CurrentMovementForce = MarinePawn->GetMovementForce();

	InitialMovementForce = CurrentMovementForce;
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

#pragma region ///////////// SLIDING ///////////////////
void UCroachAndSlide::Sliding(float Delta)
{
	if (bShouldSlide == false) 
		return;

	if (bShouldPlaySound == true && SlideSound)
	{
		SpawnedSlideSound = UGameplayStatics::SpawnSound2D(GetWorld(), SlideSound);
		bShouldPlaySound = false;
	}

	if (MarinePawn->GetJumpComponent()->GetIsOnRamp())
	{
		SlideOnRamp(Delta);
	}
	else
	{
		CurrentMovementForce -= SlideSpeed * Delta;
	}

	if (ShouldStopSliding())
	{
		CurrentMovementForce = CrouchForceSpeed;
		TurnOffSlideSound();
		if (IsValid(CameraShakeBase))
			CameraShakeBase->StopShake(false);
		bShouldSlide = false;
	}

	MarinePawn->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::SlideOnRamp(const float& Delta)
{
	if (MarinePawn->GetJumpComponent()->GetIsGoingUp() == false)
	{
		CurrentMovementForce += (CurrentMovementForce < MaxSlideForce) ? (RampForce)*Delta : 0;
		MarinePawn->SetShouldPlayerGoForward(true);

		if (bStartRampCameraShake == false)
		{
			CameraShakeBase = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(RampCameraShake, 1.f);
			bStartRampCameraShake = true;
		}
	}
	else
	{
		CurrentMovementForce -= SlideSpeedRamp * Delta;
	}
}

bool UCroachAndSlide::ShouldStopSliding()
{
	bool bPlayerIsntMoving = MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetShouldPlayerGoForward() == false && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0;
	return CurrentMovementForce <= CrouchForceSpeed || bPlayerIsntMoving;
}

void UCroachAndSlide::BeginSlide()
{
	if (MarinePawn->GetJumpComponent()->GetIsGoingUp() == true)
		return;
	if (MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0)
		return;
	if (MarinePawn->GetJumpComponent()->GetIsJumping())
		return;

	CurrentMovementForce = InitialMovementForce + InitialVelocityOfSliding;
	bShouldSlide = true;
}

void UCroachAndSlide::TurnOffSlideSound()
{
	if (!SpawnedSlideSound) return;

	SpawnedSlideSound->ToggleActive();
	SpawnedSlideSound = nullptr;
}
#pragma endregion

void UCroachAndSlide::CrouchPressed(bool bSlide)
{
	if (MarinePawn->GetIsPlayerLerpingToHookLocation() || MarinePawn->GetIsWallrunning() || bIsCrouching == true) 
		return;

	bIsCrouching = true;
	bCrouchPressed = true;

	if (bShouldStillCroach) 
		return;

	CurrentMovementForce = CrouchForceSpeed;
	ScaleZ = ScalePlayerWhenCrouching.GetLowerBoundValue();
	VignetteIntensityValue = CrouchPressedVignetteIntensity;

	bCanCroachLerp = true;
	if (bSlide)
	{
		GetWorld()->GetTimerManager().SetTimer(SlideDelayHandle, this, &UCroachAndSlide::BeginSlide, SlideDelayInSeconds, false);
	}

	MarinePawn->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::CroachLerp(float Delta)
{
	if (bCanCroachLerp == false) return;

	float NewVignetteIntensity = FMath::Lerp(MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity, VignetteIntensityValue, Delta * SpeedOfCrouchLerp);
	MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity = NewVignetteIntensity;

	float NewScaleZ = FMath::Lerp(GetOwner()->GetActorScale3D().Z, ScaleZ, Delta * SpeedOfCrouchLerp);
	GetOwner()->SetActorScale3D(FVector(2, 2, NewScaleZ));
	if (GetOwner()->GetActorScale3D().Equals(FVector(2, 2, ScaleZ), 0.01))
	{
		GetOwner()->SetActorScale3D(FVector(2, 2, ScaleZ));
		bCanCroachLerp = false;
	}
}

void UCroachAndSlide::CrouchReleased()
{
	bCrouchPressed = false;

	if (MarinePawn->GetJumpComponent()->GetIsOnRamp() || bIsCrouching == false)
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

	ScaleZ = ScalePlayerWhenCrouching.GetUpperBoundValue();
	VignetteIntensityValue = 0.f;
	bCanCroachLerp = true;

	CurrentMovementForce = InitialMovementForce;
	MarinePawn->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::CrouchReleasedByObject()
{
	if (bCrouchPressed == false)
	{
		CrouchReleased();
	}
}

bool UCroachAndSlide::SweepBox(FVector Where, float Distance)
{
	FVector Start = GetOwner()->GetActorLocation();
	Start.Z += 200.f;
	FVector End = Start + (Where * Distance);

	FHitResult HitResult;
	return GetWorld()->SweepSingleByChannel(HitResult, Start, Start, FQuat::Identity, ECC_GameTraceChannel6, FCollisionShape::MakeBox(FVector(30, 30, 60)));
}
