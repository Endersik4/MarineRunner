// Copyright Adam Bartela.All Rights Reserved

#include "CroachAndSlideComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/Movement/JumpComponent.h"

UCroachAndSlide::UCroachAndSlide()
{
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UCroachAndSlide::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	SetPlayerVariables();
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

	ChangingCrouchState(DeltaTime);
	Sliding(DeltaTime);
}

#pragma region ////////// CROUCH /////////////
void UCroachAndSlide::CrouchPressed(bool bSlidePressed)
{
	if (!IsValid(Player))
		return;

	if (Player->GetIsPlayerMovingToHookLocation() || Player->GetIsWallrunning() || bIsCrouching)
		return;

	bCrouchPressed = true;

	if (GetWorld()->GetTimerManager().IsTimerActive(CheckCrouchReleasedHandle))
		return;

	PrepareVariablesForChangingCrouchState(CrouchPlayerScale, CrouchPressedVignetteIntensity);
	bChangingCrouchState = true;

	bIsCrouching = true;
	CurrentMovementForce = CrouchSpeed;
	Player->SetMovementForce(CurrentMovementForce);

	if (bSlidePressed)
		GetWorld()->GetTimerManager().SetTimer(SlideDelayHandle, this, &UCroachAndSlide::BeginSlide, SlideDelayInSeconds, false);
}

// change actor scale accroding to the state of crouch (e.g is now standing or is now crouching) and vignette intensity for the effect
void UCroachAndSlide::ChangingCrouchState(float Delta)
{
	if (!bChangingCrouchState)
		return;

	if (!IsValid(Player))
		return;

	if (CrouchPressedTimeElapsed <= TimeToChangeCrouchState)
	{
		const float& NewVignetteIntensity = FMath::Lerp(CurrentVignetteIntensity, Target_VignetteIntensity, CrouchPressedTimeElapsed / TimeToChangeCrouchState);
		Player->GetCamera()->PostProcessSettings.VignetteIntensity = NewVignetteIntensity;

		const float& NewScaleZ = FMath::Lerp(CurrentActorScale_Z, Target_ScaleZ, CrouchPressedTimeElapsed / TimeToChangeCrouchState);
		Player->SetActorScale3D(FVector(OriginalPlayerScale.X, OriginalPlayerScale.Y, NewScaleZ));

		CrouchPressedTimeElapsed += Delta;
	}
	else
	{
		Player->SetActorScale3D(FVector(OriginalPlayerScale.X, OriginalPlayerScale.Y, Target_ScaleZ));
		bChangingCrouchState = false;
	}
}

void UCroachAndSlide::CrouchReleased()
{
	bCrouchPressed = false;

	if (!IsValid(Player))
		return;

	//if (Player->GetJumpComponent()->GetIsOnRamp() || !bIsCrouching)
	//	return;

	if (!CanPlayerStand())
		return;
	StopCrouching();
}

bool UCroachAndSlide::CanPlayerStand()
{
	if (!IsValid(Player))
		return false;

	FHitResult ObstacleAboveResult;
	const FVector& CheckObstacleLocation = Player->GetRoofLocationSceneComponent()->GetComponentLocation();
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

	PrepareVariablesForChangingCrouchState(OriginalPlayerScale.Z, 0.f);
	bChangingCrouchState = true;

	CurrentMovementForce = OriginalMovementForce;

	if (!IsValid(Player))
		return;

	Player->SetMovementForce(OriginalMovementForce);
}

void UCroachAndSlide::PrepareVariablesForChangingCrouchState(const float& TargetScale_Z, const float& TargetVignetteIntensity)
{
	CrouchPressedTimeElapsed = 0.f;
	CurrentVignetteIntensity = Player->GetCamera()->PostProcessSettings.VignetteIntensity;
	CurrentActorScale_Z = Player->GetActorScale3D().Z;

	Target_ScaleZ = TargetScale_Z;
	Target_VignetteIntensity = TargetVignetteIntensity;
}

void UCroachAndSlide::CrouchReleasedByObject()
{
	if (bCrouchPressed)
		return;

	CrouchReleased();
}
#pragma endregion

#pragma region ///////////// SLIDING ///////////////////
void UCroachAndSlide::BeginSlide()
{
	if (!IsValid(Player))
		return;

	// Cant slide if going uphill
	if (Player->GetJumpComponent()->GetIsGoingUp())
		return;

	// Cant Slide backward
	if (Player->GetInputAxisValue(TEXT("Forward")) != 1.f && Player->GetInputAxisValue(TEXT("Right")) == 0)
		return;

	if (Player->GetJumpComponent()->GetIsJumping() || Player->GetJumpComponent()->GetIsInAir())
		return;

	CurrentMovementForce = OriginalMovementForce + InitialVelocityOfSliding;
	bSlide = true;
}

void UCroachAndSlide::Sliding(float Delta)
{
	if (!bSlide) 
		return;

	if (!IsValid(Player))
		return;

	TurnOnSlideSound();

	if (Player->GetJumpComponent()->GetIsOnRamp())
		SlideOnRamp(Delta);
	else
		CurrentMovementForce -= SlideSpeed * Delta;

	if (ShouldStopSliding())
		StopSliding();

	Player->SetMovementForce(CurrentMovementForce);
}

void UCroachAndSlide::SlideOnRamp(const float& Delta)
{
	if (!IsValid(Player))
		return;

	if (Player->GetJumpComponent()->GetIsGoingUp())
	{
		CurrentMovementForce -= RampSlideUpForce * Delta;
		Player->SetShouldPlayerGoForward(false);

		return;
	}

	// if current movement force is bigger then max force then do not add more force 
	CurrentMovementForce += (CurrentMovementForce < MaxSlideForce) ? RampSlideDownForce * Delta : 0;
	Player->SetShouldPlayerGoForward(true);

	if (!bRampCameraShakeStarted && IsValid(PlayerController))
	{
		CameraShakeBase = PlayerController->PlayerCameraManager->StartCameraShake(RampCameraShake);
		bRampCameraShakeStarted = true;
	}
}

// if player stopped moving or current movement force is smaller ten crouch force speed then stop sliding
bool UCroachAndSlide::ShouldStopSliding()
{
	if (!IsValid(Player))
		return false;

	const bool bPlayerIsntMoving = Player->GetInputAxisValue(TEXT("Forward")) != 1.f && Player->GetShouldPlayerGoForward() == false && Player->GetInputAxisValue(TEXT("Right")) == 0;
	return CurrentMovementForce <= CrouchSpeed || bPlayerIsntMoving;
}

void UCroachAndSlide::StopSliding()
{
	CurrentMovementForce = CrouchSpeed;
	TurnOffSlideSound();
	StopSlideCameraShake();

	bSlide = false;
}

void UCroachAndSlide::StopSlideCameraShake()
{
	if (!IsValid(CameraShakeBase))
		return;

	if (!CameraShakeBase->IsActive())
		return;

	CameraShakeBase->StopShake(false);
}

void UCroachAndSlide::TurnOnSlideSound()
{
	if (!bShouldPlaySound || !IsValid(SlideSound))
		return;

	SpawnedSlideSound = UGameplayStatics::SpawnSound2D(GetWorld(), SlideSound);
	bShouldPlaySound = false;
}

void UCroachAndSlide::TurnOffSlideSound()
{
	if (!IsValid(SpawnedSlideSound)) 
		return;

	SpawnedSlideSound->ToggleActive();
	SpawnedSlideSound = nullptr;
}
#pragma endregion

void UCroachAndSlide::SetPlayerVariables()
{
	if (!ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Jump Component!")))
		return;

	Player = Cast<AMarineCharacter>(GetOwner());

	if (!IsValid(Player))
		return;

	CurrentMovementForce = Player->GetMovementForce();
	OriginalMovementForce = CurrentMovementForce;
	OriginalPlayerScale = Player->GetActorScale3D();

	Player->GetCamera()->PostProcessSettings.bOverride_VignetteIntensity = true;
	Player->OnDestroyed.AddUniqueDynamic(this, &UCroachAndSlide::OnOwnerDestroyed);
}