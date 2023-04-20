// Fill out your copyright notice in the Description page of Project Settings.


#include "JumpPhysics.h"

// Sets default values for this component's properties
UJumpPhysics::UJumpPhysics()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UJumpPhysics::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UJumpPhysics::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//MarineFalling(DeltaTime);
	//Jumping(DeltaTime);
}


void UJumpPhysics::Jump()
{
	if (bShouldJump == false && JumpsLeft > 0)
	{
		CopyJumpVelocityStart = JumpVelocityStart;
		JumpsLeft--;
		bShouldJump = true;
	}
}

void UJumpPhysics::Jumping(float Delta)
{
	if (bShouldJump == false) return;

	FVector Location = GetOwner()->GetActorLocation();
	FVector CopyLocation = Location;
	Location.Z += (JumpVelocityStart * Delta);
	bool bIsSomeThingUpStairs = GetOwner()->SetActorLocation(Location, true);

	JumpVelocityStart -= (SubstructVelocityOnJumping * Delta);
	if (JumpVelocityStart <= 0 || (GetOwner()->GetActorLocation().Z - CopyLocation.Z >= 0.01 && GetOwner()->GetActorLocation().Z - CopyLocation.Z <= 0.05))
	{
		bShouldJump = false;
		JumpVelocityStart = CopyJumpVelocityStart;
	}
}

void UJumpPhysics::MarineFalling(float Delta)
{
	if (bShouldJump == false)
	{
		FVector Location = GetOwner()->GetActorLocation();
		Location.Z += (Falling * Delta);
		bIsFalling = GetOwner()->SetActorLocation(Location, true);
		if (bIsFalling == true)
		{
			Falling -= (SubstructVelocityOnFalling * Delta);
		}
		else
		{
			JumpsLeft = 2;
			Falling = -200;
		}
	}
}