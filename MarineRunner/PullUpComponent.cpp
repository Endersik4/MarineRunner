// Fill out your copyright notice in the Description page of Project Settings.


#include "PullUpComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineCharacter.h"

// Sets default values for this component's properties
UPullUpComponent::UPullUpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPullUpComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (MarinePawn == nullptr) UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));
}


// Called every frame
void UPullUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MarinePawn)
	{
		if (MarinePawn->GetIsInAir())
		{
			EdgePullUp();
		}
		PullupLerp(DeltaTime);
	}
}

void UPullUpComponent::EdgePullUp()
{
	if (PulledHimselfUp) return;

	//Locations of Lines 
	FVector StartTrueLine = MarinePawn->GetActorLocation();
	StartTrueLine.Z += PullupTrueLineZ;
	FVector StartFalseLine = MarinePawn->GetActorLocation();
	StartFalseLine.Z += PullupFalseLineZ;
	FHitResult Line1Hit, Line2Hit;

	if (MakeCheckLine(Line1Hit, StartTrueLine, StartTrueLine + MarinePawn->GetActorForwardVector() * 100.f) == true &&
		MakeCheckLine(Line2Hit, StartFalseLine, StartFalseLine + MarinePawn->GetActorForwardVector() * 100.f) == false)
	{
		//DrawDebugLine(GetWorld(), StartTrueLine, StartTrueLine + GetActorForwardVector() * 100.f, FColor::Red, true);
		//DrawDebugLine(GetWorld(), StartFalseLine, StartFalseLine + GetActorForwardVector() * 100.f, FColor::Magenta, true);
		MarinePawn->SetShouldAddCounterMovement(true);

		//Setting a line that is in the direction of the object the player wants to pull up.
		//The Line is like this: /
		FHitResult HitResult;
		TArray<AActor*> ActorsToIgnore;
		FVector LineStart = MarinePawn->GetActorLocation();
		LineStart.Z += 150.f;
		LineStart += MarinePawn->GetActorForwardVector() * 100.f;
		FVector LineEnd = LineStart + (MarinePawn->GetActorUpVector() * 200.f);

		if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
		{
			PullupLocationZ = MarinePawn->GetActorLocation();
			PullupLocationZ.Z = HitResult.ImpactPoint.Z + 75.f;

			ShouldPullUpLerp = true;
			MarineLocation = MarinePawn->GetActorLocation();
			PullupTimeElapsed = 0.f;
		}
		PulledHimselfUp = true;
	}

}

void UPullUpComponent::PullupLerp(float Delta)
{
	if (!ShouldPullUpLerp) return;

	if (PullupTimeElapsed < PullUpTime)
	{
		FVector LerpLocation = FMath::Lerp(MarineLocation, PullupLocationZ, PullupTimeElapsed / PullUpTime);
		MarinePawn->SetActorLocation(LerpLocation);
		PullupTimeElapsed += Delta;
	}
	else
	{
		ShouldPullUpLerp = false;
		PulledHimselfUp = false;
		FVector Impulse = MarinePawn->GetActorForwardVector() * PullUpForceForward * 100;
		Impulse.Z += PullUpForceUp * 100;
		MarinePawn->CapsulePawn->AddImpulse(Impulse);
	}
}

bool UPullUpComponent::MakeCheckLine(FHitResult& OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug, FColor Color)
{
	if (bDebug) DrawDebugLine(GetWorld(), NewStart, NewEnd, Color, false, 0.5f);
	return GetWorld()->LineTraceSingleByChannel(OutHitResult, NewStart, NewEnd, ECC_Visibility);
}
