// Copyright Adam Bartela.All Rights Reserved

#include "WallrunComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"

UWallrunComponent::UWallrunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UWallrunComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!IsValid(MarinePawn)) 
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN WALLRUN COMPONENT!"));
}

void UWallrunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Wallrunning(DeltaTime);
	CameraRotationTowardsHitNormal(DeltaTime);
	RotateCameraRollTimeline.TickTimeline(DeltaTime);
}

void UWallrunComponent::Wallrunning(float Delta)
{
	//Checking if Pawn Can do wallrun
	if (!CanDoWallrun(Delta))
	{
		return;
	}

	ESideOfLine CurrentSide;
	FVector HitNormal; //Hit.ImpactNormal Vector from HitResult of Obstacle
	if (IsPawnNextToObstacle(HitNormal, CurrentSide) || IsPawnNextToObstacle(HitNormal, CurrentSide, ESideOfLine::Right))
	{
		PlayerRotationWhileWallrun = MarinePawn->GetActorRotation();
		StickToTheObstacle(CurrentSide, HitNormal);
	}
	else if (bIsWallrunning)
	{
		ResetWallrunning(); //If There is no obstacle around then Wallrun should be disabled
		MarinePawn->GetPlayerCapsule()->AddImpulse(WallrunningWhereToJump * JumpFromWallrunImpulse);
	}
}

void UWallrunComponent::StickToTheObstacle(ESideOfLine CurrentSide, FVector HitNormal)
{
	//The player must move forward to perform the wallrun
	if (!bIsWallrunning && !(MarinePawn->GetInputAxisValue("Forward") > 0.5f))
		return;

	BeginWallrun(CurrentSide, HitNormal);
	WallrunningWhereToJump = HitNormal;

	if (MarinePawn->GetVelocity().Length() > VelocityRangeToStopWallrunming.GetLowerBoundValue() && MarinePawn->GetVelocity().Length() < VelocityRangeToStopWallrunming.GetUpperBoundValue())
	{
		ResetWallrunning();
		return;
	}

	//added Impulse to Stick with Obstacle
	FVector Impulse = (-HitNormal) * StickWithObstacleImpulse;
	MarinePawn->GetPlayerCapsule()->AddImpulse(Impulse);
}

void UWallrunComponent::BeginWallrun(ESideOfLine CurrentSide, FVector HitNormal)
{
	if (bIsWallrunning)
		return;

	WallrunTimeElapsed = DelayToStartNextWallrun;

	MarinePawn->SetShouldPlayerGoForward(true);
	MarinePawn->SetMovementSpeedMutliplier(WallrunSpeed); //Player goes faster while performing wallrun

	CurrentRotatedCameraRoll = CurrentSide;
	RotateCameraWhileWallrunning(CurrentSide == Right ? CameraRollRightSideCurve : CameraRollLeftSideCurve);//Rotating the camera in Roll, Definition of this function is in Blueprint of MarineCharacter

	RotateCameraYaw(CurrentSide, HitNormal);

	float YawMovementImpulse = HitNormal.Rotation().Yaw + (AngleOfHitImpact * (CurrentSide == Left ? -1 : 1));
	WallrunDirection = FRotator(0, YawMovementImpulse, 0).Vector();

	bIsWallrunning = true;

	bCanJumpWhileWallrunning = false;
	GetWorld()->GetTimerManager().SetTimer(CanJumpHandle, this, &UWallrunComponent::SetCanJumpWhileWallrunning, CanJumpWhenWallrunStartsTime);
}

bool UWallrunComponent::IsPawnNextToObstacle(FVector& HitNormal, ESideOfLine& OutCurrentSide, ESideOfLine WhichSideToLook)
{
	FHitResult ObstacleHitResult;
	FVector EndDirection = (WhichSideToLook == ESideOfLine::Right ? 1.f : -1.f) * (MarinePawn->GetRootComponent()->GetRightVector() * ObstacleRaycastDistance);

	FVector StartLowerRaycastLocation = MarinePawn->GetActorLocation();
	FVector EndLowerRaycastLocation = StartLowerRaycastLocation + EndDirection;
	bool bLowerRaycast = GetWorld()->LineTraceSingleByChannel(ObstacleHitResult, StartLowerRaycastLocation, EndLowerRaycastLocation, ECC_GameTraceChannel4);
	if (!bLowerRaycast)
		return false;

	FVector StartUpperRaycastLocation = MarinePawn->GetRoofLocationSceneComponent()->GetComponentLocation();
	FVector EndUpperRaycastLocation = StartUpperRaycastLocation + EndDirection;
	bool bUpperRaycast = GetWorld()->LineTraceSingleByChannel(ObstacleHitResult, StartUpperRaycastLocation, EndUpperRaycastLocation, ECC_GameTraceChannel4);
	if (!bUpperRaycast)
		return false;

	HitNormal = ObstacleHitResult.ImpactNormal;
	OutCurrentSide = WhichSideToLook;

	return true;
}

void UWallrunComponent::ResetWallrunning()
{
	WallrunTimeElapsed = 0.f;
	bIsWallrunning = false;
	bRotateYawCameraTowardsWallrun = false;

	MarinePawn->SetShouldPlayerGoForward(false);
	MarinePawn->SetMovementSpeedMutliplier(1.f);

	RotateCameraWhileWallrunning(CurrentRotatedCameraRoll == Right ? CameraRollRightSideCurve : CameraRollLeftSideCurve);

	if (MarinePawn->GetJumpComponent()->GetIsJumping())
		return;

	bCanJumpAfterWallrun = true;
	GetWorld()->GetTimerManager().SetTimer(CanJumpAfterWallrunHandle, this, &UWallrunComponent::DisableCanJumpAfterWallrun, CanJumpAfterWallrunTime, false);
}

void UWallrunComponent::DisableCanJumpAfterWallrun()
{
	bCanJumpAfterWallrun = false;
}

bool UWallrunComponent::CanDoWallrun(float Delta)
{
	if (!IsValid(MarinePawn))
		return false;

	if (!MarinePawn->GetIsInAir())
		return false;

	if (WallrunTimeElapsed < DelayToStartNextWallrun) //Wait a little bit before the next wallrun
	{
		if (bIsWallrunning && !MarinePawn->GetJumpComponent()->GetIsJumping())
			ResetWallrunning();
		else 
			WallrunTimeElapsed += Delta;

		return false;
	}

	if (MarinePawn->GetIsPullingUp())
		return false;
	if (MarinePawn->GetIsCrouching())
		return false;
	// only take X and Y velocity for length
	if ((MarinePawn->GetVelocity() * FVector(1.f, 1.f, 0.f)).Length() < MinVelocityToPerformWallrun)
		return false;

	//if player looks far away from the wall while wallrunning it then stop wallruning
	float DistanceBetweenYaws = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(PlayerRotationWallrun, PlayerRotationWhileWallrun).Yaw);
	if (bIsWallrunning && DistanceBetweenYaws > MaxYawDistanceToStopWallrunning)
	{
		ResetWallrunning();
		return false;
	}
	return true;
}

void UWallrunComponent::AddImpulseAfterWallrun()
{
	if (!bIsWallrunning)
		return;

	ResetWallrunning();
	MarinePawn->GetPlayerCapsule()->AddImpulse(WallrunningWhereToJump * JumpFromWallrunImpulse);
}

void UWallrunComponent::CallResetWallrunningAfterLanding()
{
	if (!bIsWallrunning) 
		return;

	ResetWallrunning();
}

void UWallrunComponent::RotateCameraYaw(ESideOfLine CurrentSide, FVector HitNormal)
{
	bRotateYawCameraTowardsWallrun = true;
	RotateYawCameraAngle = HitNormal.Rotation().Yaw + ((CurrentSide == Left ? -1 : 1) * AngleOfHitImpact);
	PlayerRotationWallrun.Yaw = RotateYawCameraAngle;
}

void UWallrunComponent::CameraRotationTowardsHitNormal(float Delta)
{
	if (!bRotateYawCameraTowardsWallrun || !IsValid(PlayerController))
		return;
	FRotator CurrentRotation = PlayerController->GetControlRotation();
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Yaw = RotateYawCameraAngle;

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, Delta, CameraYawSpeed);
	PlayerController->SetControlRotation(NewRotation);
}

void UWallrunComponent::SetCanJumpWhileWallrunning()
{
	bCanJumpWhileWallrunning = true;
}

void UWallrunComponent::RotateCameraWhileWallrunning(UCurveFloat* CurveToUse)
{
	if (!CurveToUse|| !IsValid(PlayerController))
		return;

	FOnTimelineFloat RotateCameraRollTimelineProgress;
	RotateCameraRollTimelineProgress.BindUFunction(this, FName("CameraRollTimelineProgress"));
	RotateCameraRollTimeline.AddInterpFloat(CurveToUse, RotateCameraRollTimelineProgress);

	if (bCameraRollWasRotated)
	{
		RotateCameraRollTimeline.ReverseFromEnd();
		bCameraRollWasRotated = false;
	}
	else
	{
		RotateCameraRollTimeline.PlayFromStart();
		bCameraRollWasRotated = true;
	}
}

void UWallrunComponent::CameraRollTimelineProgress(float CurveValue)
{
	FRotator NewRotation = PlayerController->GetControlRotation();
	NewRotation.Roll = CurveValue;
	PlayerController->SetControlRotation(NewRotation);
}