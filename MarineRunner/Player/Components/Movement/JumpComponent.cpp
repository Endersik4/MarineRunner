// Copyright Adam Bartela.All Rights Reserved


#include "JumpComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/Movement/WallrunComponent.h"
#include "MarineRunner/Player/Components/Movement/CroachAndSlideComponent.h"

UJumpComponent::UJumpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UJumpComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Jump Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}

	GetWorld()->GetTimerManager().SetTimer(CheckInAirHandle, this, &UJumpComponent::CheckIfIsInAir, CheckInAirTime, true);
	CreateImpactOnFloorTimeline();
}

void UJumpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyJumpForces(DeltaTime);
	ImpactOnFloorTimeline.TickTimeline(DeltaTime);
}

#pragma region ////////////////////////////////// JUMP /////////////////////////////////
bool UJumpComponent::CanJump()
{
	FHitResult SomethingUpResult;
	const bool bObstacleAbovePlayer = GetWorld()->SweepSingleByChannel(SomethingUpResult, Player->GetRoofLocationSceneComponent()->GetComponentLocation(), Player->GetRoofLocationSceneComponent()->GetComponentLocation(), FQuat::Identity, ECollisionChannel::ECC_GameTraceChannel11, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsUp));
	
	if (bObstacleAbovePlayer)
		return false;

	if (Player->GetWallrunComponent()->GetIsWallrunning())
		return true;

	if (bStartApplyingJumpForces)
		return false;

	if (bIsInAir && !bDelayIsInAir && !Player->GetWallrunComponent()->GetCanJumpAfterWallrun())
		return false;

	return true;
}

void UJumpComponent::Jump()
{
	if (!IsValid(Player))
		return;

	if (!Player->GetWallrunComponent()->GetCanJump())
		return;

	if (CanJump())
	{
		Player->GetWallrunComponent()->SetCanJumpAfterWallrun(false);
		Player->GetWallrunComponent()->AddImpulseAfterWallrun();
		Player->GetCrouchAndSlideComponent()->CrouchReleasedByObject();

		bStartApplyingJumpForces = true;
		JumpTimeElapsed = 0.f;

		if (IsValid(JumpSound))
			UGameplayStatics::SpawnSound2D(GetWorld(), JumpSound);
		else
			UE_LOG(LogTemp, Warning, TEXT("Jump Sound is nullptr in Jump Cpmponent!"));
	}
	else
	{
		bCanDelayJump = true;
		GetWorld()->GetTimerManager().SetTimer(DelayJumpHandle, this, &UJumpComponent::DisableCanDelayJump, DelayJumpTime, false);
	}
}

void UJumpComponent::ApplyJumpForces(const float& DeltaTime)
{
	if (!bStartApplyingJumpForces || !IsValid(Player))
		return;

	if (JumpTimeElapsed < JumpUpTime)
	{
		ApplyJumpUPForce(DeltaTime);
	}
	else if (!bDownForceWasApplied) // for faster falling after jumping
	{
		const FVector& DownJumpImpulse = (-Player->GetActorUpVector() * JumpDownForce);
		Player->GetPlayerCapsule()->AddImpulse(DownJumpImpulse);

		bDownForceWasApplied = true;
	}
	else
	{
		bDelayIsInAir = false;
		bDownForceWasApplied = false;
		bStartApplyingJumpForces = false;
	}
}

void UJumpComponent::ApplyJumpUPForce(const float& DeltaTime)
{
	JumpTimeElapsed += DeltaTime;

	const FVector& CheckObstacleLocation = Player->GetRoofLocationSceneComponent()->GetComponentLocation();
	FHitResult ObstacleAboveHitResult;
	const bool bObstacleAbovePlayer = GetWorld()->SweepSingleByChannel(ObstacleAboveHitResult, CheckObstacleLocation, CheckObstacleLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsUp));
	if (bObstacleAbovePlayer)
	{
		// if player hits soemthing uphead then stop applying jump forces
		JumpTimeElapsed += JumpUpTime;
		return;
	}

	const float& NewVel_Z = FMath::Lerp(InitialJumpForce, EndJumpForce, JumpTimeElapsed / JumpUpTime);
	FVector JumpVelocity = Player->GetPlayerCapsule()->GetPhysicsLinearVelocity();
	JumpVelocity.Z = NewVel_Z;
	Player->GetPlayerCapsule()->SetPhysicsLinearVelocity(JumpVelocity);
}

void UJumpComponent::TurnOffJump(bool bDelayJumpToo)
{
	bStartApplyingJumpForces = false;

	if (!bDelayJumpToo) 
		return;

	DelayJump();
}

void UJumpComponent::DelayJump()
{
	if (!bCanDelayJump) 
		return;

	Jump();
	bCanDelayJump = false;
	GetWorld()->GetTimerManager().ClearTimer(DelayJumpHandle);
}
#pragma endregion 

#pragma region ////////////////////////////////// AIR //////////////////////////////////
void UJumpComponent::CheckIfIsInAir()
{
	if (!IsValid(Player))
		return;

	FHitResult GroundHitResult;
	const FVector& GroundCheckLocation = Player->GetGroundLocationSceneComponent()->GetComponentLocation();
	//Check if there is ground under the player, if not, the player is in the air
	const bool bSomethingIsBelowThePlayer = GetWorld()->SweepSingleByChannel(GroundHitResult, GroundCheckLocation, GroundCheckLocation, FQuat::Identity, ECC_GameTraceChannel11, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsBelow));
	if (!bSomethingIsBelowThePlayer)
	{
		if (bIsInAir)
			return;

		FirstMomentInAir();
	}
	else
	{
		FirstTimeOnGround();

		PlayerOnRamp(GroundHitResult);

		if (GroundHitResult.GetActor()->ActorHasTag(TEXT("Elevator"))) // cant save while in elevator
			Player->bIsPlayerInElevator = true;
		else 
			Player->bIsPlayerInElevator = false;

		DelayJump();
	}
}

void UJumpComponent::FirstMomentInAir()
{
	bDelayIsInAir = true;
	GetWorld()->GetTimerManager().SetTimer(DelayIsInAirHandle, this, &UJumpComponent::DisableDelayIsInAir, DelayIsInAirTime, false);

	DisablePlayerOnRampActions();

	bIsInAir = true;
}

void UJumpComponent::FirstTimeOnGround()
{
	if (!bIsInAir|| !IsValid(Player))
		return;

	Player->GetWallrunComponent()->CallResetWallrunningAfterLanding();

	bIsInAir = false;
	bDelayIsInAir = false;

	if (IsValid(ImpactOnFloorSound)) 
		UGameplayStatics::SpawnSoundAttached(ImpactOnFloorSound, Player->GetPlayerCapsule());
	else
		UE_LOG(LogTemp, Warning, TEXT("Impact On Floor Sound is nullptr in Jump Cpmponent!"));

	LandingEffect();
}

void UJumpComponent::PlayerOnRamp(const FHitResult& GroundHitResult)
{
	if (!GroundHitResult.GetActor()->ActorHasTag("Ramp"))
	{
		DisablePlayerOnRampActions();
		return;
	}

	if (!bIsOnRamp)
	{
		bIsOnRamp = true;

		if (!Player->GetCrouchAndSlideComponent()->GetIsSliding())
		{
			Player->SetShouldPlayerGoForward(true);
			Player->GetCrouchAndSlideComponent()->CrouchPressed();
			Player->GetCrouchAndSlideComponent()->BeginSlide();
		}
	}

	//Check if Pawn is going UP on ramp, if he is then he cant slide
	if (PreviousPlayerLocationOnRamp.Z < Player->GetActorLocation().Z)
	{
		bIsGoingUpOnRamp = true;
	}
	else if (bIsGoingUpOnRamp)
	{
		bIsOnRamp = false;
		Player->SetShouldPlayerGoForward(false);
		
		bIsGoingUpOnRamp = false;
	}
	PreviousPlayerLocationOnRamp = Player->GetActorLocation();
}

void UJumpComponent::DisablePlayerOnRampActions()
{
	if (!bIsOnRamp)
		return;

	bIsOnRamp = false;
	bIsGoingUpOnRamp = false;
	Player->SetShouldPlayerGoForward(false);
	Player->GetCrouchAndSlideComponent()->CrouchReleasedByObject();
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
	if (!IsValid(ImpactOnFloorCameraEffectCurve))
		return;

	ImpactOnFloorTimeline.PlayFromStart();
}

void UJumpComponent::ImpactOnFloorCameraEffectTimelineProgress(float CurveValue)
{
	FVector NewCameraLocation = Player->GetCamera()->GetRelativeLocation();
	NewCameraLocation.Z = CurveValue;
	Player->GetCamera()->SetRelativeLocation(NewCameraLocation);
}
#pragma endregion

