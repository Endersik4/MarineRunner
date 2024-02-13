// Copyright Adam Bartela.All Rights Reserved

#include "DashComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/Widgets/DashWidget.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LerpToDashLocation(DeltaTime);
}

void UDashComponent::Dash()
{
	if (CanPlayerPerformDash() == false)
		return;

	bIsPerformingDash = true;
	bCanDash = false;

	InitialPlayerPosition = MarinePawn->GetActorLocation();
	DashLocation = CalculateEndDashPosition();

	DashEffects();
}

FVector UDashComponent::CalculateEndDashPosition()
{
	FHitResult HitResult;
	bool bHit = GetCloserHitResult(HitResult);

	// If object that was hit is closer then DashDistance then calculate time for new distance
	CalculatedDashTime = bHit ? (HitResult.Distance * DashTime) / DashDistance : DashTime;

	FVector EndRaycastLocation = InitialPlayerPosition + (CalculateDashDirection() * DashDistance);
	return (bHit ? HitResult.Location : EndRaycastLocation) - CalculateDashDirection() * OffsetFromObstacle;
}

bool UDashComponent::GetCloserHitResult(FHitResult& OutHitResult)
{
	FVector EndRaycastLowerLoc = InitialPlayerPosition + (CalculateDashDirection() * DashDistance);
	FVector EndRaycastHigherLoc = MarinePawn->GetCameraLocation() + (CalculateDashDirection() * DashDistance);

	// if there is obstacle on the way then set actor location to Hit Location with offset
	FHitResult HitResultLower, HitResultHigher;
	bool bHitLower = GetWorld()->LineTraceSingleByChannel(HitResultLower, InitialPlayerPosition, EndRaycastLowerLoc, ECC_GameTraceChannel7);
	bool bHitHigher = GetWorld()->LineTraceSingleByChannel(HitResultHigher, MarinePawn->GetCameraLocation(), EndRaycastHigherLoc, ECC_GameTraceChannel7);

	if (bHitLower == true || bHitHigher == true)
	{
		HitResultHigher.Location.Z = InitialPlayerPosition.Z;
		if (bHitHigher == false)
		{
			OutHitResult = HitResultLower;
		}
		else if (bHitLower == false)
		{
			OutHitResult = HitResultHigher;
		}
		else if (HitResultLower.Distance <= HitResultHigher.Distance)
		{
			OutHitResult = HitResultLower;
		}
		else
			OutHitResult = HitResultHigher;

		return true;
	}
	else 
		return false;
}

const FVector UDashComponent::CalculateDashDirection()
{
	const float ForwardVectorDegree = -90.f;
	FVector PlayerForwardVector = MarinePawn->GetController()->GetRootComponent()->GetRightVector().RotateAngleAxis(ForwardVectorDegree, FVector(0.f, 0.f, 1.f));

	FVector CalculatedDashDirection = MarinePawn->GetController()->GetRootComponent()->GetRightVector() * MarinePawn->GetInputAxisValue("Right") + PlayerForwardVector * MarinePawn->GetInputAxisValue("Forward");
	return CalculatedDashDirection;
}

void UDashComponent::DashEffects()
{
	if (DashSound) UGameplayStatics::SpawnSound2D(GetWorld(), DashSound);

	UDashWidget* DashWidget = Cast<UDashWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), DashWidgetClass));
	if (IsValid(DashWidget))
		DashWidget->AddToViewport();

	if (IsValid(MarinePawn) == false)
		return;

	ElementBar DashElementBar{ DashCoolDown };
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::Dash, DashElementBar);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_Dash);
}

void UDashComponent::LerpToDashLocation(float Delta)
{
	if (bIsPerformingDash == false)
		return;

	if (DashTimeElapsed <= CalculatedDashTime)
	{
		FVector NewLoc = FMath::Lerp(InitialPlayerPosition, DashLocation, DashTimeElapsed / CalculatedDashTime);
		MarinePawn->SetActorLocation(NewLoc);
	}
	else
	{
		TurnOffDash();
	}
	DashTimeElapsed += (Delta / UGameplayStatics::GetGlobalTimeDilation(GetWorld()));
}

void UDashComponent::TurnOffDash()
{
	DashTimeElapsed = 0.f;
	bIsPerformingDash = false;
	GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle, this, &UDashComponent::EndDashCooldown, DashCoolDown, false);
}

bool UDashComponent::CanPlayerPerformDash() const
{
	if (IsValid(MarinePawn) == false || bCanDash == false)
		return false;

	if (MarinePawn->GetIsPlayerLerpingToHookLocation() == true || MarinePawn->GetIsWallrunning() == true) 
		return false;

	if (MarinePawn->GetIsCrouching() == true )
		return false;

	if (MarinePawn->GetInputAxisValue("Right") == 0.f && MarinePawn->GetInputAxisValue("Forward") == 0.f)
		return false;

	return true;
}