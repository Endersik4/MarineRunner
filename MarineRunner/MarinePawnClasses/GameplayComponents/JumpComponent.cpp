// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/JumpComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WallrunComponent.h"
#include "MarineRunner/MarinePawnClasses/PullUpComponent.h"

UJumpComponent::UJumpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UJumpComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());
	InitialMovementForce = Player->GetMovementForce();
}

void UJumpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	JumpTick(DeltaTime);
	CheckIfIsInAir();
}

#pragma region ////////////////////////////////// JUMP /////////////////////////////////
void UJumpComponent::Jump()
{
	if (Player->GetWallrunComponent()->GetCanJump() == false) 
		return;

	if (bIsInAir == false || (bDelayIsInAir && bIsJumping == false) || Player->GetWallrunComponent()->ShouldAddImpulseAfterWallrun(true))
	{
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
		FVector Start = Player->GetActorLocation();
		Start.Z += 100.f * Player->GetActorScale3D().Z;
		FHitResult HitR;

		bool bObstacleAbovePlayer = GetWorld()->SweepSingleByChannel(HitR, Start, Start, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeBox(FVector(4.4, 4.4, 8.5)));
		if (bObstacleAbovePlayer == false)
		{
			//Jumps Up
			float NewVelZ = FMath::Lerp(InitialJumpForce, -50.f, JumpTimeElapsed / JumpUpTime);

			FVector LinearVelocity = Player->CapsulePawn->GetPhysicsLinearVelocity();
			LinearVelocity.Z = NewVelZ;

			Player->CapsulePawn->SetPhysicsLinearVelocity(LinearVelocity);
		}
		else JumpTimeElapsed += JumpUpTime;

		Player->GetWallrunComponent()->AddImpulseAfterWallrun(JumpTimeElapsed);

		JumpTimeElapsed += DeltaTime;
	}
	else if (bDownForce == false)
	{
		//Down Physics applied when TimeJump is over
		FVector DownJumpImpulse = (-Player->GetActorUpVector() * JumpDownForce * 10);
		//if (GetIsInSlowMotion() == true) DownJumpImpulse *= 3.f;
		Player->CapsulePawn->AddImpulse(DownJumpImpulse);

		bDownForce = true;
	}
	else
	{
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
	//Check if there is ground under the player, if not, the player is in the air
	bool bSomethingIsBelowThePlayer = GetWorld()->SweepSingleByChannel(GroundHitResult, Player->GetActorLocation(), Player->GetActorLocation(), FQuat::Identity, ECC_Visibility, FCollisionShape::MakeBox(BoxSizeToCheckIfSomethingIsBelow));
	if (bSomethingIsBelowThePlayer == false)
	{
		//The first moment a player is in the air
		if (bIsInAir == false)
		{
			bDelayIsInAir = true;
			GetWorld()->GetTimerManager().SetTimer(DelayIsInAirHandle, this, &UJumpComponent::SetDelayIsInAir, DelayIsInAirTime, false);
		}
		bIsInAir = true;
		bIsOnRamp = false;
	}
	else
	{
		FirstTimeOnGround();

		PlayerOnRamp(GroundHitResult);

		DelayJump();
	}
}

void UJumpComponent::FirstTimeOnGround()
{
	if (bIsInAir == false || IsValid(Player) == false)
		return;

	Player->GetWallrunComponent()->CallResetWallrunningAfterLanding();

	bIsInAir = false;
	bDelayIsInAir = false;
	Player->GetPullUpComponent()->SetPulledHimselfUp(false);

	if (ImpactOnFloorSound) 
		UGameplayStatics::SpawnSoundAttached(ImpactOnFloorSound, Player->CapsulePawn);

	if (Player->GetIsCrouching() == false) 
		Player->SetMovementForce(InitialMovementForce);

	Player->LandingEffect();
}

void UJumpComponent::PlayerOnRamp(const FHitResult& GroundHitResult)
{
	if (GroundHitResult.GetActor()->ActorHasTag("Ramp"))
	{
		bIsOnRamp = true;
		//Check if Pawn is going UP on ramp, if he is then he cant slide
		if (!GroundHitResult.GetActor()->GetActorForwardVector().Equals(Player->GetActorForwardVector(), 1.1f))
		{
			bIsGoingUp = true;
		}
		else
		{
			bIsGoingUp = false;
		}
	}
	else if (bIsOnRamp == true)
	{
		bIsOnRamp = false;
	}
}

#pragma endregion

