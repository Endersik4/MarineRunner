// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"
#include "MarineRunner/MarinePawnClasses/CroachAndSlide.h"

UJumpComponent::UJumpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UJumpComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());

	CreateImpactOnFloorTimeline();
}

void UJumpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	JumpTick(DeltaTime);
	CheckIfIsInAir();
	ImpactOnFloorTimeline.TickTimeline(DeltaTime);
}

#pragma region ////////////////////////////////// JUMP /////////////////////////////////
bool UJumpComponent::CanJump()
{
	FHitResult SomethinhUpResult;
	bool bObstacleAbovePlayer = GetWorld()->SweepSingleByChannel(SomethinhUpResult, Player->GetRoofLocationSceneComponent()->GetComponentLocation(), Player->GetRoofLocationSceneComponent()->GetComponentLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel11, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsUp));
	
	if (bObstacleAbovePlayer == true)
		return false;

	return bIsInAir == false || ((bDelayIsInAir || Player->GetWallrunComponent()->GetCanJumpAfterWallrun()) && bIsJumping == false) || Player->GetWallrunComponent()->ShouldAddImpulseAfterWallrun(true);
}

void UJumpComponent::Jump()
{
	if (Player->GetWallrunComponent()->GetCanJump() == false)
		return;

	if (CanJump())
	{
		Player->GetWallrunComponent()->SetCanJumpAfterWallrun(false);
		bIsJumping = true;
		JumpTimeElapsed = 0;
		if (JumpSound) UGameplayStatics::SpawnSound2D(GetWorld(), JumpSound);
	}
	else
	{
		bCanDelayJump = true;
		GetWorld()->GetTimerManager().SetTimer(DelayJumpHandle, this, &UJumpComponent::SetCanDelayJump, DelayJumpTime, false);
	}
}

void UJumpComponent::JumpTick(float DeltaTime)
{
	if (bIsJumping == false || IsValid(Player) == false)
		return;

	if (JumpTimeElapsed < JumpUpTime)
	{
		FVector CheckObstacleLocation = Player->GetRoofLocationSceneComponent()->GetComponentLocation();
		FHitResult HitR;

		bool bObstacleAbovePlayer = GetWorld()->SweepSingleByChannel(HitR, CheckObstacleLocation, CheckObstacleLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsUp));
		if (bObstacleAbovePlayer == false)
		{
			//Jumps Up
			float NewVelZ = FMath::Lerp(InitialJumpForce, -50.f, JumpTimeElapsed / JumpUpTime);

			FVector LinearVelocity = Player->GetPlayerCapsule()->GetPhysicsLinearVelocity();
			LinearVelocity.Z = NewVelZ;

			Player->GetPlayerCapsule()->SetPhysicsLinearVelocity(LinearVelocity);
		}
		else
		{
			JumpTimeElapsed += JumpUpTime;
		}

		Player->GetWallrunComponent()->AddImpulseAfterWallrun(JumpTimeElapsed);

		JumpTimeElapsed += DeltaTime;
	}
	else if (bDownForce == false)
	{
		//Down Physics applied when TimeJump is over
		FVector DownJumpImpulse = (-Player->GetActorUpVector() * JumpDownForce * 10);
		Player->GetPlayerCapsule()->AddImpulse(DownJumpImpulse);

		bDownForce = true;
	}
	else
	{
		bDelayIsInAir = false;
		bDownForce = false;
		bIsJumping = false;
	}
}

void UJumpComponent::TurnOffJump(bool bDelayJumpToo)
{
	bIsJumping = false;

	if (bDelayJumpToo == false) 
		return;

	if (bCanDelayJump) 
		DelayJump();
}

void UJumpComponent::DelayJump()
{
	if (bCanDelayJump == false) return;

	Jump();
	bCanDelayJump = false;
	GetWorld()->GetTimerManager().ClearTimer(DelayJumpHandle);
}
#pragma endregion 

#pragma region ////////////////////////////////// AIR //////////////////////////////////
void UJumpComponent::CheckIfIsInAir()
{
	FHitResult GroundHitResult;
	FVector GroundCheckLocation = Player->GetGroundLocationSceneComponent()->GetComponentLocation();
	//Check if there is ground under the player, if not, the player is in the air
	bool bSomethingIsBelowThePlayer = GetWorld()->SweepSingleByChannel(GroundHitResult, GroundCheckLocation, GroundCheckLocation, FQuat::Identity, ECC_GameTraceChannel11, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsBelow));
	if (bSomethingIsBelowThePlayer == false)
	{
		if (bIsInAir == true)
			return;

		FirstMomentInAir();
	}
	else
	{
		FirstTimeOnGround();

		PlayerOnRamp(GroundHitResult);

		if (GroundHitResult.GetActor()->ActorHasTag(TEXT("Elevator")))
		{
			Player->bIsPlayerInElevator = true;
		}
		else Player->bIsPlayerInElevator = false;

		DelayJump();
	}
}

void UJumpComponent::FirstMomentInAir()
{
	bDelayIsInAir = true;
	GetWorld()->GetTimerManager().SetTimer(DelayIsInAirHandle, this, &UJumpComponent::SetDelayIsInAir, DelayIsInAirTime, false);

	DisablePlayerOnRampActions();

	bIsInAir = true;
}

void UJumpComponent::FirstTimeOnGround()
{
	if (bIsInAir == false || IsValid(Player) == false)
		return;

	Player->GetWallrunComponent()->CallResetWallrunningAfterLanding();

	bIsInAir = false;
	bDelayIsInAir = false;

	if (ImpactOnFloorSound) 
		UGameplayStatics::SpawnSoundAttached(ImpactOnFloorSound, Player->GetPlayerCapsule());

	LandingEffect();
}

void UJumpComponent::PlayerOnRamp(const FHitResult& GroundHitResult)
{
	if (GroundHitResult.GetActor()->ActorHasTag("Ramp"))
	{
		if (bIsOnRamp == false)
		{
			bIsOnRamp = true;

			if (Player->GetCroachAndSlideComponent()->GetIsSliding() == false)
			{
				Player->SetShouldPlayerGoForward(true);
				Player->GetCroachAndSlideComponent()->BeginSlide();
				Player->GetCroachAndSlideComponent()->CrouchPressed();
			}
		}

		//Check if Pawn is going UP on ramp, if he is then he cant slide
		if (PlayerLocationOnRamp.Z < Player->GetActorLocation().Z)
		{
			bIsGoingUp = true;
		}
		else if (bIsGoingUp == true)
		{
			bIsOnRamp = false;
			Player->SetShouldPlayerGoForward(false);

			bIsGoingUp = false;
		}
		PlayerLocationOnRamp = Player->GetActorLocation();
	}
	else DisablePlayerOnRampActions();
}

void UJumpComponent::DisablePlayerOnRampActions()
{
	if (bIsOnRamp == false)
		return;

	bIsOnRamp = false;
	bIsGoingUp = false;
	Player->SetShouldPlayerGoForward(false);
	Player->GetCroachAndSlideComponent()->CrouchReleasedByObject();
}
#pragma endregion

#pragma region //////////// Impact On Floor Effect ////////////
void UJumpComponent::CreateImpactOnFloorTimeline()
{
	FOnTimelineFloat ImpactOnFloorTimelineProgress;
	ImpactOnFloorTimelineProgress.BindUFunction(this, FName("ImpactOnFloorCameraEffectTimelineProgress"));
	ImpactOnFloorTimeline.AddInterpFloat(ImpactOnFloorCameraEffectCurve, ImpactOnFloorTimelineProgress);
}

void UJumpComponent::LandingEffect()
{
	if (ImpactOnFloorCameraEffectCurve == nullptr)
		return;

	ImpactOnFloorTimeline.PlayFromStart();
}

void UJumpComponent::ImpactOnFloorCameraEffectTimelineProgress(float CurveValue)
{
	FVector NewLocation = Player->GetCamera()->GetRelativeLocation();
	NewLocation.Z = CurveValue;
	Player->GetCamera()->SetRelativeLocation(NewLocation);
}
#pragma endregion

