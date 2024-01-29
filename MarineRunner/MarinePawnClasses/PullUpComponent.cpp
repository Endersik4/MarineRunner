// Copyright Adam Bartela.All Rights Reserved

#include "PullUpComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values for this component's properties
UPullUpComponent::UPullUpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPullUpComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (IsValid(MarinePawn) == false) 
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));

	GetWorld()->GetTimerManager().SetTimer(CheckIfShouldPullUpHandle, this, &UPullUpComponent::StartPullUpOnEdge, CheckLinesForPullUpTime, true);
}


// Called every frame
void UPullUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MovePlayerToPullUpLocation(DeltaTime);
}

bool UPullUpComponent::DetectEdge(const FVector& PlayerForwardVector)
{
	FVector StartTrueLine = MarinePawn->GetActorLocation() + FVector(0.f, 0.f, PullupTrueLineZ);
	FVector StartFalseLine = MarinePawn->GetActorLocation() + FVector(0.f, 0.f, PullupFalseLineZ);
	FHitResult Line1Hit, Line2Hit;

	bool bObjectWasDetected = GetWorld()->LineTraceSingleByChannel(Line1Hit, StartTrueLine, StartTrueLine + PlayerForwardVector * PullupLinesDistance, ECC_GameTraceChannel8);
	bool bStillObjectIsDetected = GetWorld()->LineTraceSingleByChannel(Line1Hit, StartFalseLine, StartFalseLine + PlayerForwardVector * PullupLinesDistance, ECC_GameTraceChannel8);

	#ifdef WITH_EDITOR
		if (bDrawHelpersForPullup == true)
		{
			DrawDebugLine(GetWorld(), StartTrueLine, StartTrueLine + PlayerForwardVector * PullupLinesDistance, FColor::Green, true);
			DrawDebugLine(GetWorld(), StartFalseLine, StartFalseLine + PlayerForwardVector * PullupLinesDistance, FColor::Red, true);
		}
	#endif // !WITH_EDITOR

	return bObjectWasDetected && bStillObjectIsDetected == false;
}

void UPullUpComponent::StartPullUpOnEdge()
{
	if (IsValid(MarinePawn) == false)
		return;

	if (bShouldPullUpLerp || MarinePawn->GetIsInAir() == false)
		return;

	FVector PlayerForwardVector = CalculateForwardVectorForPlayer();

	if (DetectEdge(PlayerForwardVector) == false)
		return;

	FVector ImpactPoint;
	if (GetEdgeLocationOfTheObject(PlayerForwardVector, ImpactPoint) == false)
		return;

	PullupLocationZ = MarinePawn->GetActorLocation();
	PullupLocationZ.Z = ImpactPoint.Z + PullupOffset_Z;
	PlayerLocation = MarinePawn->GetActorLocation();
	bShouldPullUpLerp = true;
	PullupTimeElapsed = 0.f;

	#ifdef WITH_EDITOR
		if(bDrawHelpersForPullup) 
			DrawDebugBox(GetWorld(), PullupLocationZ, FVector(10.f), FColor::Blue, true);
	#endif // !WITH_EDITOR
}

bool UPullUpComponent::GetEdgeLocationOfTheObject(const FVector& PlayerForwardVector, FVector& EdgeLocation)
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	FVector LineStart = (MarinePawn->GetActorLocation() + FVector(0.f, 0.f, PullupTrueLineZ)) + PlayerForwardVector * 100.f;
	FVector LineEnd = LineStart + (MarinePawn->GetActorUpVector() * 200.f);

	bool bObjectHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), LineStart, LineEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel8), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	if (bObjectHit == true)
	{
		EdgeLocation = HitResult.ImpactPoint;
		return true;
	}
	
	return false;
}

void UPullUpComponent::MovePlayerToPullUpLocation(float Delta)
{
	if (!bShouldPullUpLerp) 
		return;

	if (IsValid(MarinePawn) == false)
		return;

	if (PullupTimeElapsed <= PullUpTime)
	{
		FVector LerpLocation = FMath::Lerp(PlayerLocation, PullupLocationZ, PullupTimeElapsed / PullUpTime);
		MarinePawn->SetActorLocation(LerpLocation);

		PullupTimeElapsed += Delta;
	}
	else
	{
		MarinePawn->GetPlayerCapsule()->SetPhysicsLinearVelocity(FVector(0.f));
		FVector Impulse = (CalculateForwardVectorForPlayer() * PullUpForceForward) + FVector(0.f, 0.f, PullUpForceUp);
		MarinePawn->GetPlayerCapsule()->AddImpulse(Impulse);

		bShouldPullUpLerp = false;
	}
}

const FVector UPullUpComponent::CalculateForwardVectorForPlayer()
{
	const float ForwardVectorDegree = -90.f;
	return MarinePawn->GetController()->GetRootComponent()->GetRightVector().RotateAngleAxis(ForwardVectorDegree, FVector(0.f, 0.f, 1.f));
}
