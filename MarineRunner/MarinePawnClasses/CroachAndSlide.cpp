// Copyright Adam Bartela.All Rights Reserved

#include "CroachAndSlide.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"

UCroachAndSlide::UCroachAndSlide()
{
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UCroachAndSlide::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	CurrentMovementForce = MarinePawn->GetMovementForce();

	OriginalMovementForce = CurrentMovementForce;
	MarinePawn->GetCamera()->PostProcessSettings.bOverride_VignetteIntensity = true;

	MarinePawn->OnDestroyed.AddDynamic(this, &UCroachAndSlide::OnOwnerDestroyed);

}

void UCroachAndSlide::OnOwnerDestroyed(AActor* DestroyedActor)
{
	if (IsValid(SpawnedSlideSound))
		SpawnedSlideSound->Stop();
}

// Called every frame
void UCroachAndSlide::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	StartChangingCrouchState(DeltaTime);
	Sliding(DeltaTime);
}

#pragma region ///////////// SLIDING ///////////////////
void UCroachAndSlide::BeginSlide()
{
	// Cant slide if going uphill
	if (MarinePawn->GetJumpComponent()->GetIsGoingUp() == true)
	{
		return;
	}
	// Cant Slide backward
	if (MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0)
	{
		return;
	}
	if (MarinePawn->GetJumpComponent()->GetIsJumping())
	{
		return;
	}

	CurrentMovementForce = OriginalMovementForce + InitialVelocityOfSliding;
	bSlide = true;
}

void UCroachAndSlide::Sliding(float Delta)
{
	if (bSlide == false) 
		return;

	TurnOnSlideSound();

	if (MarinePawn->GetJumpComponent()->GetIsOnRamp())
	{
		SlideOnRamp(Delta);
	}
	else
		CurrentMovementForce -= SlideSpeed * Delta;

	if (ShouldStopSliding())
	{
		StopSliding();
	}

	MarinePawn->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::SlideOnRamp(const float& Delta)
{
	if (MarinePawn->GetJumpComponent()->GetIsGoingUp() == true)
	{
		CurrentMovementForce -= SlideSpeedRamp * Delta;
		return;
	}

	// if current movement force is bigger then max force then do not add more force 
	CurrentMovementForce += (CurrentMovementForce < MaxSlideForce) ? RampForce*Delta : 0;
	MarinePawn->SetShouldPlayerGoForward(true);

	if (bRampCameraShakeStarted == false)
	{
		CameraShakeBase = UGameplayStatics::GetPlayerController(GetWorld(), 0)->PlayerCameraManager->StartCameraShake(RampCameraShake, 1.f);
		bRampCameraShakeStarted = true;
	}
}

// if player stopped moving or current movement force is smaller ten crouch force speed then stop sliding
bool UCroachAndSlide::ShouldStopSliding()
{
	bool bPlayerIsntMoving = MarinePawn->GetInputAxisValue(TEXT("Forward")) != 1.f && MarinePawn->GetShouldPlayerGoForward() == false && MarinePawn->GetInputAxisValue(TEXT("Right")) == 0;
	return CurrentMovementForce <= CrouchForceSpeed || bPlayerIsntMoving;
}

void UCroachAndSlide::StopSliding()
{
	CurrentMovementForce = CrouchForceSpeed;
	TurnOffSlideSound();
	StopSlideCameraShake();

	bSlide = false;
}

void UCroachAndSlide::StopSlideCameraShake()
{
	if (IsValid(CameraShakeBase) == false)
		return;

	if (CameraShakeBase->IsActive() == false)
		return;

	CameraShakeBase->StopShake(false);
}

void UCroachAndSlide::TurnOnSlideSound()
{
	if (bShouldPlaySound == false || SlideSound == nullptr)
		return;

	SpawnedSlideSound = UGameplayStatics::SpawnSound2D(GetWorld(), SlideSound);
	bShouldPlaySound = false;
}

void UCroachAndSlide::TurnOffSlideSound()
{
	if (IsValid(SpawnedSlideSound) == false) 
		return;

	SpawnedSlideSound->ToggleActive();
	SpawnedSlideSound = nullptr;
}
#pragma endregion

#pragma region ////////// CROUCH /////////////
void UCroachAndSlide::CrouchPressed(bool bSlidePressed)
{
	if (MarinePawn->GetIsPlayerLerpingToHookLocation() || MarinePawn->GetIsWallrunning() || bIsCrouching == true) 
		return;

	bCrouchPressed = true;

	if (GetWorld()->GetTimerManager().IsTimerActive(CheckCrouchReleasedHandle) == true)
		return;

	PrepareVariablesForChangingCrouchState(ScalePlayerWhenCrouching.GetLowerBoundValue(), CrouchPressedVignetteIntensity);
	bIsNowChangingCrouchState = true;

	if (bSlidePressed)
		GetWorld()->GetTimerManager().SetTimer(SlideDelayHandle, this, &UCroachAndSlide::BeginSlide, SlideDelayInSeconds, false);

	bIsCrouching = true;
	CurrentMovementForce = CrouchForceSpeed;
	MarinePawn->SetMovementForce(CurrentMovementForce);
}

// change actor scale accroding to the state of crouch (e.g is now standing or is now crouching) and vignette intensity for the effect
void UCroachAndSlide::StartChangingCrouchState(float Delta)
{
	if (bIsNowChangingCrouchState == false) 
		return;

	if (CrouchPressedTimeElapsed <= TimeToChangeCrouchState)
	{
		float NewVignetteIntensity = FMath::Lerp(CurrentVignetteIntensity, Target_VignetteIntensity, CrouchPressedTimeElapsed / TimeToChangeCrouchState);
		MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity = NewVignetteIntensity;

		float NewScaleZ = FMath::Lerp(CurrentActorScale_Z, Target_ScaleZ, CrouchPressedTimeElapsed / TimeToChangeCrouchState);
		MarinePawn->SetActorScale3D(FVector(1, 1, NewScaleZ));

		CrouchPressedTimeElapsed += Delta;
	}
	else
	{
		MarinePawn->SetActorScale3D(FVector(1, 1, Target_ScaleZ));
		bIsNowChangingCrouchState = false;
	}
}

void UCroachAndSlide::CrouchReleased()
{
	bCrouchPressed = false;

	if (MarinePawn->GetJumpComponent()->GetIsOnRamp() || bIsCrouching == false)
		return;

	if (bCanPlayerStand() == false)
		return;

	StopCrouching();
}

bool UCroachAndSlide::bCanPlayerStand()
{
	FHitResult ObstacleAboveResult;
	FVector CheckObstacleLocation = MarinePawn->GetRoofLocationSceneComponent()->GetComponentLocation();
	if (GetWorld()->SweepSingleByChannel(ObstacleAboveResult, CheckObstacleLocation, CheckObstacleLocation, FQuat::Identity, ECC_GameTraceChannel6, FCollisionShape::MakeBox(CheckBoxSizeToStandUpAfterCrouch)))
	{
		GetWorld()->GetTimerManager().SetTimer(CheckCrouchReleasedHandle, this, &UCroachAndSlide::CrouchReleased, CrouchReleasedTimeCheck, false);
		return false;
	}

	return true;
}

void UCroachAndSlide::StopCrouching()
{
	bIsCrouching = false;
	bShouldPlaySound = true;

	bSlide = false;
	TurnOffSlideSound();
	GetWorld()->GetTimerManager().ClearTimer(SlideDelayHandle);

	bRampCameraShakeStarted = false;
	StopSlideCameraShake();

	PrepareVariablesForChangingCrouchState(ScalePlayerWhenCrouching.GetUpperBoundValue(), 0.f);
	bIsNowChangingCrouchState = true;

	CurrentMovementForce = OriginalMovementForce;
	MarinePawn->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::PrepareVariablesForChangingCrouchState(const float& TargetScale_Z, const float& TargetVignetteIntensity)
{
	CrouchPressedTimeElapsed = 0.f;
	CurrentVignetteIntensity = MarinePawn->GetCamera()->PostProcessSettings.VignetteIntensity;
	CurrentActorScale_Z = MarinePawn->GetActorScale3D().Z;

	Target_ScaleZ = TargetScale_Z;
	Target_VignetteIntensity = TargetVignetteIntensity;
}

void UCroachAndSlide::CrouchReleasedByObject()
{
	if (bCrouchPressed == true)
		return;
	
	CrouchReleased();
}

#pragma endregion