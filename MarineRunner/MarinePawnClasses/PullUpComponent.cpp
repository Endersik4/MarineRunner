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
	if (!IsValid(MarinePawn)) 
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));

	GetWorld()->GetTimerManager().SetTimer(CheckIfShouldPullUpHandle, this, &UPullUpComponent::StartPullUpOnEdge, CheckLinesForPullUpTime, true);
}


// Called every frame
void UPullUpComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MovePlayerToPullUpLocation(DeltaTime);
}

void UPullUpComponent::StartPullUpOnEdge()
{
	if (!IsValid(MarinePawn))
		return;

	if (bIsPullingUp || !MarinePawn->GetIsInAir())
		return;

	FVector PlayerForwardVector = CalculateForwardVectorForPlayer();
	if (!DetectEdge(PlayerForwardVector))
		return;

	FVector ImpactPoint;
	if (!GetEdgeLocationOfTheObject(PlayerForwardVector, ImpactPoint))
		return;

	PullupLocationZ = MarinePawn->GetActorLocation();
	PullupLocationZ.Z = ImpactPoint.Z + PullupOffset_Z;
	PlayerLocation = MarinePawn->GetActorLocation();
	bIsPullingUp = true;
	PullupTimeElapsed = 0.f;

#ifdef WITH_EDITOR
	if (bDrawHelpersForPullup)
		DrawDebugBox(GetWorld(), PullupLocationZ, FVector(10.f), FColor::Blue, true);
#endif // !WITH_EDITOR
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

bool UPullUpComponent::GetEdgeLocationOfTheObject(const FVector& PlayerForwardVector, FVector& EdgeLocation)
{
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	FVector EdgeDetectionLineStart = (MarinePawn->GetActorLocation() + FVector(0.f, 0.f, PullupTrueLineZ)) + PlayerForwardVector * EdgeDetectionLineStartDistance;
	FVector EdgeDetectionLineEnd = EdgeDetectionLineStart + (MarinePawn->GetActorUpVector() * EdgeDetectionLineEndDistance);

	bool bObjectHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), EdgeDetectionLineStart, EdgeDetectionLineEnd, UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel8), false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
	
	if (!bObjectHit)
		return false;

	EdgeLocation = HitResult.ImpactPoint;
	return true;
}

void UPullUpComponent::MovePlayerToPullUpLocation(float Delta)
{
	if (!bIsPullingUp) 
		return;

	if (!IsValid(MarinePawn))
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

		bIsPullingUp = false;
	}
}

const FVector UPullUpComponent::CalculateForwardVectorForPlayer()
{
	const float ForwardVectorDegree = -90.f;
	return MarinePawn->GetController()->GetRootComponent()->GetRightVector().RotateAngleAxis(ForwardVectorDegree, FVector(0.f, 0.f, 1.f));
}
