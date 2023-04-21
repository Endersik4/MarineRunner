// Fill out your copyright notice in the Description page of Project Settings.


#include "WallrunComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values for this component's properties
UWallrunComponent::UWallrunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWallrunComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN WALLRUN COMPONENT!"));
}


// Called every frame
void UWallrunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	//Calling Wallrunning functione when Player is In Air
	if (MarinePawn->GetIsInAir())
	{
		Wallrunning();
	}

	CameraRotationInterp(DeltaTime);
}

void UWallrunComponent::Wallrunning()
{
	//Checking if Pawn Can do wallrun
	if (!ShouldDoWallrun()) return;

	int32 Side; //On which side the obstacle is, -1 = left, 1 = right
	FVector HitNormal; //Hit.ImpactNormal Vector from HitResult of Obstacle
	if (IsPawnNextToObstacle(Side, HitNormal))
	{
		StickToTheObstacle(Side, HitNormal);
	}
	else  if (bIsWallrunning) ResetWallrunning(); //If There is no obstacle around then Wallrun should be disabled
}

void UWallrunComponent::StickToTheObstacle(int32 WhichSide, FVector HitNormal)
{
	if (bIsWallrunning == false)
	{
		//The player must move forward to perform the wallrun
		if (UKismetMathLibrary::InRange_FloatFloat(MarinePawn->GetInputAxisValue(TEXT("Forward")), -0.1, 0.1)) return;
		WallrunTimeElapsed = 0.6f;

		//Setting up MarinePawn varialbes
		MarinePawn->SetMovementSpeedMutliplier(WallrunSpeed); //Player goes faster while performing wallrun
		MarinePawn->RotateCameraWhileWallrunning(WhichSide == 1 ? true : false);//Rotating the camera in Roll, Definition of this function is in Blueprint of MarineCharacter

		RotateCameraYaw(1 * WhichSide, HitNormal);
		MarinePawn->SetMovementImpulse(FRotator(0, HitNormal.Rotation().Yaw + (85 * WhichSide), 0).Vector());

		bShouldPlayerGoForward = true;
		bIsWallrunning = true;

		bCanJumpWhileWallrunning = false;
		GetWorld()->GetTimerManager().SetTimer(CanJumpHandle, this, &UWallrunComponent::SetCanJumpWhileWallrunning, 0.2f);

		//Things that cannot happen while WAllrunning
		MarinePawn->MovementStuffThatCannotHappen();
	}
	WallrunningWhereToJump = HitNormal;

	//Impulse added to Stick with Obstacle
	FVector Impulse = (-HitNormal) * StickWithObstacleImpulse * 100.f;
	MarinePawn->CapsulePawn->AddImpulse(Impulse);
}

void UWallrunComponent::ResetWallrunning()
{
	WallrunTimeElapsed = 0.f;
	bShouldAddImpulseAfterWallrun = false;
	bIsWallrunning = false;
	bShouldPlayerGoForward = false;
	bShouldLerpRotation = false;

	MarinePawn->SetMovementSpeedMutliplier(1.f);
	MarinePawn->SetShouldAddCounterMovement(false);
	MarinePawn->RotateCameraWhileWallrunning();
}

bool UWallrunComponent::IsPawnNextToObstacle(int32& WhichSide, FVector& HitNormal)
{
	//Creating Locations of Lines
	FVector CheckLineStart = MarinePawn->GetCamera()->GetComponentLocation(); //Upper Line
	FVector CheckSecondLineStart = MarinePawn->GetActorLocation(); //Lower Line

	FVector LeftCheckLineEnd = CheckLineStart + (-MarinePawn->GetActorRightVector() * 150.f);
	FVector LeftCheckSecondLineEnd = CheckSecondLineStart + (-MarinePawn->GetActorRightVector() * 150.f);
	FVector RightCheckLineEnd = CheckLineStart + (MarinePawn->GetActorRightVector() * 150.f);
	FVector RightCheckSecondLineEnd = CheckSecondLineStart + (MarinePawn->GetActorRightVector() * 150.f);

	FHitResult HitResult;
	FHitResult Hit;
	//Checking if Two Lines hit something (Left Side)
	if (MakeCheckLine(Hit, CheckLineStart, LeftCheckLineEnd) && MakeCheckLine(HitResult, CheckSecondLineStart, LeftCheckSecondLineEnd))
	{
		WhichSide = -1;
		HitNormal = Hit.ImpactNormal;
		return true;
	}
	//Checking if Two Lines hit something (Right Side)
	else if (MakeCheckLine(Hit, CheckLineStart, RightCheckLineEnd) && MakeCheckLine(HitResult, CheckSecondLineStart, RightCheckSecondLineEnd))
	{
		WhichSide = 1;
		HitNormal = Hit.ImpactNormal;
		return true;
	}
	return false;
}

bool UWallrunComponent::ShouldDoWallrun()
{
	if (WallrunTimeElapsed < 0.2f) //Wait a little bit before the next wallrun
	{
		if (bIsWallrunning && MarinePawn->GetIsJumping() == false) ResetWallrunning();
		else WallrunTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		return false;
	}
	return true;
}

bool UWallrunComponent::ShouldAddImpulseAfterWallrun(bool bShould)
{
	if (!bIsWallrunning) return false;
	bShouldAddImpulseAfterWallrun = bShould;
	return true;
}

void UWallrunComponent::AddImpulseAfterWallrun(float JumpTimeElapsed)
{
	if (JumpTimeElapsed > 0.02f && bShouldAddImpulseAfterWallrun == true)
	{
		ResetWallrunning();
		MarinePawn->CapsulePawn->AddImpulse(WallrunningWhereToJump * 1200000.f);
	}
}

void UWallrunComponent::CallResetWallrunningAfterLanding()
{
	if (!bIsWallrunning) return;

	ResetWallrunning();
}

void UWallrunComponent::RotateCameraYaw(int32 WhichSide, FVector HitNormal)
{
	bShouldLerpRotation = true;
	WhereToInterp = HitNormal.Rotation().Yaw + (WhichSide * AngleOfHitImpact);
}

void UWallrunComponent::CameraRotationInterp(float Delta)
{
	if (!bShouldLerpRotation) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FRotator CurrentRotation = PC->GetControlRotation();
	//FRotator CurrentRotation = MarinePawn->GetActorRotation();
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Yaw = WhereToInterp;

	FRotator NewRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, Delta, CameraYawSpeed);
	PC->SetControlRotation(NewRotation);
	//MarinePawn->SetActorRotation(NewRotation);
	//In Blueprint BP_MarinePlayerController when player moves mouse (with some tolerance) then bShouldLerpRotation = false
}

void UWallrunComponent::SetCanJumpWhileWallrunning()
{
	bCanJumpWhileWallrunning = true;
}

bool UWallrunComponent::MakeCheckLine(FHitResult& OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug, FColor Color)
{
	if (bDebug) DrawDebugLine(GetWorld(), NewStart, NewEnd, Color, false, 0.5f);
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, NewStart, NewEnd, ECC_Visibility);
}
