// Copyright Adam Bartela.All Rights Reserved

#include "WallrunComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
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

	if (IsValid(MarinePawn) == false) UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN WALLRUN COMPONENT!"));
}

void UWallrunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Calling Wallrunning functione when Player is In Air
	if (MarinePawn->GetIsInAir())
	{
		Wallrunning(DeltaTime);
	}
	CameraRotationInterp(DeltaTime);
	RotateCameraRollTimeline.TickTimeline(DeltaTime);
}

void UWallrunComponent::Wallrunning(float Delta)
{
	//Checking if Pawn Can do wallrun
	if (CanDoWallrun(Delta) == false) return;

	ESideOfLine CurrentSide;
	FVector HitNormal; //Hit.ImpactNormal Vector from HitResult of Obstacle
	if (IsPawnNextToObstacle(HitNormal, CurrentSide) || IsPawnNextToObstacle(HitNormal, CurrentSide, ESideOfLine::Right))
	{
		PlayerRotationWhileWallrun = MarinePawn->GetActorRotation();
		StickToTheObstacle(CurrentSide, HitNormal);
	}
	else  if (bIsWallrunning)
	{
		ResetWallrunning(); //If There is no obstacle around then Wallrun should be disabled
		MarinePawn->GetPlayerCapsule()->AddImpulse(WallrunningWhereToJump * JumpFromWallrunImpulse);
	}
}

void UWallrunComponent::StickToTheObstacle(ESideOfLine CurrentSide, FVector HitNormal)
{
	//The player must move forward to perform the wallrun
	if (bIsWallrunning == false && MarinePawn->GetInputAxisValue("Forward") > 0.5f == false)
		return;

	BeginWallrun(CurrentSide, HitNormal);
	WallrunningWhereToJump = HitNormal;

	if (MarinePawn->GetVelocity().Length() > VelocityRangeToStopWallrunming.GetLowerBoundValue() && MarinePawn->GetVelocity().Length() < VelocityRangeToStopWallrunming.GetUpperBoundValue())
	{
		ResetWallrunning();
		return;
	}

	//added Impulse to Stick with Obstacle
	FVector Impulse = (-HitNormal) * StickWithObstacleImpulse * 100.f;
	MarinePawn->GetPlayerCapsule()->AddImpulse(Impulse);
}

void UWallrunComponent::BeginWallrun(ESideOfLine CurrentSide, FVector HitNormal)
{
	if (bIsWallrunning == true)
		return;

	WallrunTimeElapsed = 0.6f;

	MarinePawn->MovementStuffThatCannotHappen(true); //Things that cannot happen while Wallrunning
	MarinePawn->SetShouldPlayerGoForward(true);
	//Setting up MarinePawn variables
	MarinePawn->SetMovementSpeedMutliplier(WallrunSpeed); //Player goes faster while performing wallrun
	CurrentRotatedCameraRoll = CurrentSide;
	RotateCameraWhileWallrunning(CurrentSide == Right ? CameraRollRightSideCurve : CameraRollLeftSideCurve);//Rotating the camera in Roll, Definition of this function is in Blueprint of MarineCharacter

	RotateCameraYaw(CurrentSide, HitNormal);
	float YawMovementImpulse = HitNormal.Rotation().Yaw + (85 * (CurrentSide == Left ? -1 : 1));
	WallrunDirection = FRotator(0, YawMovementImpulse, 0).Vector();

	bIsWallrunning = true;

	bCanJumpWhileWallrunning = false;
	GetWorld()->GetTimerManager().SetTimer(CanJumpHandle, this, &UWallrunComponent::SetCanJumpWhileWallrunning, 0.1f);
}

bool UWallrunComponent::IsPawnNextToObstacle(FVector& HitNormal, ESideOfLine& OutCurrentSide, ESideOfLine WhichSideToLook)
{
	TArray<FVector> StartLocationOfLinesTrace;
	TArray<AActor*> ignor;
	TArray<FVector> EndLocationOfLinesTrace;
	auto DeleteArrays = [&StartLocationOfLinesTrace, &EndLocationOfLinesTrace]() {
		StartLocationOfLinesTrace.Empty();
		EndLocationOfLinesTrace.Empty();
	};

	//Starting Locations for LineTrace
	StartLocationOfLinesTrace.Add(MarinePawn->GetCamera()->GetComponentLocation()); //Start of Upper Line in the player
	StartLocationOfLinesTrace.Add(MarinePawn->GetActorLocation()); //Start of Lower Line in the player

	//StartLocationOfLinesTrace.Add(MarinePawn->GetCamera()->GetComponentLocation() + MarinePawn->GetActorForwardVector() * 450.f); //Start of Upper Line ahead of the player 
	//StartLocationOfLinesTrace.Add(MarinePawn->GetActorLocation() + MarinePawn->GetActorForwardVector() * 450.f); //Start of Lower Line ahead of the player

	int32 HowManyBools = 0;
	FHitResult HitResult;
	for (int i = 0; i != StartLocationOfLinesTrace.Num(); i++)
	{
		//Set end location of lines, Start Location have End Location at the same index
		if (WhichSideToLook == Left) EndLocationOfLinesTrace.Add(StartLocationOfLinesTrace[i] + (-MarinePawn->GetActorRightVector() * 150.f));
		else EndLocationOfLinesTrace.Add(StartLocationOfLinesTrace[i] + (MarinePawn->GetActorRightVector() * 150.f));

		//If Line hit wall then Add 1 to HowManyBools
		HowManyBools += UKismetSystemLibrary::LineTraceSingle(GetWorld(), StartLocationOfLinesTrace[i], EndLocationOfLinesTrace[i], UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel4), false, ignor, EDrawDebugTrace::None, HitResult, true);
		if (HitResult.GetActor()) if (HitResult.GetActor()->ActorHasTag("NoWall")) HowManyBools--;
		if (i == 0) HitNormal = HitResult.ImpactNormal; //Take out HitResult from the first line

		//If all lines hit something (HowManyBools == 4) then the player can do a wallrun
		//If The Player is wallrunning then only the first two lines count if they hit something
		//if (HowManyBools == 4 || (bIsWallrunning && HowManyBools == 2 && i == 2))
		if (HowManyBools == 2)
		{
			OutCurrentSide = WhichSideToLook;
			DeleteArrays();
			return true;
		}
	}
	DeleteArrays();
	return false;
}

void UWallrunComponent::ResetWallrunning()
{
	WallrunTimeElapsed = 0.f;
	bShouldAddImpulseAfterWallrun = false;
	bIsWallrunning = false;
	MarinePawn->SetShouldPlayerGoForward(false);
	bShouldLerpRotation = false;
	MarinePawn->SetMovementSpeedMutliplier(1.f);

	RotateCameraWhileWallrunning(CurrentRotatedCameraRoll == Right ? CameraRollRightSideCurve : CameraRollLeftSideCurve);

	if (MarinePawn->GetJumpComponent()->GetIsJumping() == true)
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
	if (WallrunTimeElapsed < 0.3f) //Wait a little bit before the next wallrun
	{
		if (bIsWallrunning && MarinePawn->GetJumpComponent()->GetIsJumping() == false) ResetWallrunning();
		else WallrunTimeElapsed += Delta;
		return false;
	}

	if (MarinePawn->GetIsInPullUpMode() == true)
		return false;
	if (MarinePawn->GetIsCrouching() == true)
		return false;
	if ((MarinePawn->GetVelocity() * FVector(1.f, 1.f, 0.f)).Length() < MinVelocityToPerformWallrun)
		return false;

	//if player looks far away from the wall while wallrunning it then stop wallruning
	float DistanceBetweenYaws = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(PlayerRotationWallrun, PlayerRotationWhileWallrun).Yaw);
	if (bIsWallrunning && DistanceBetweenYaws > 45.f)
	{
		ResetWallrunning();
		return false;
	}
	return true;
}

bool UWallrunComponent::ShouldAddImpulseAfterWallrun(bool bShould)
{
	if (!bIsWallrunning) 
		return false;
	bShouldAddImpulseAfterWallrun = bShould;
	return true;
}

void UWallrunComponent::AddImpulseAfterWallrun(float JumpTimeElapsed)
{
	if (JumpTimeElapsed > 0.02f && bShouldAddImpulseAfterWallrun == true)
	{
		ResetWallrunning();
		MarinePawn->GetPlayerCapsule()->AddImpulse(WallrunningWhereToJump * JumpFromWallrunImpulse);
	}
}

void UWallrunComponent::CallResetWallrunningAfterLanding()
{
	if (!bIsWallrunning) 
		return;

	ResetWallrunning();
}

void UWallrunComponent::RotateCameraYaw(ESideOfLine CurrentSide, FVector HitNormal)
{
	bShouldLerpRotation = true;
	WhereToInterp = HitNormal.Rotation().Yaw + ((CurrentSide == Left ? -1 : 1) * AngleOfHitImpact);
	PlayerRotationWallrun.Yaw = WhereToInterp;
}

void UWallrunComponent::CameraRotationInterp(float Delta)
{
	if (bShouldLerpRotation == false|| IsValid(PlayerController) == false)
		return;
	FRotator CurrentRotation = PlayerController->GetControlRotation();
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Yaw = WhereToInterp;

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, Delta, CameraYawSpeed);
	PlayerController->SetControlRotation(NewRotation);
	//In Blueprint BP_MarinePlayerController when player moves mouse (with some tolerance) then bShouldLerpRotation = false
}

void UWallrunComponent::SetCanJumpWhileWallrunning()
{
	bCanJumpWhileWallrunning = true;
}

void UWallrunComponent::RotateCameraWhileWallrunning(UCurveFloat* CurveToUse)
{
	if (CurveToUse == nullptr || IsValid(PlayerController) == false)
		return;

	FOnTimelineFloat RotateCameraRollTimelineProgress;
	RotateCameraRollTimelineProgress.BindUFunction(this, FName("CameraRollTimelineProgress"));
	RotateCameraRollTimeline.AddInterpFloat(CurveToUse, RotateCameraRollTimelineProgress);

	if (bCameraRollWasRotated == true)
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