// Copyright Adam Bartela.All Rights Reserved

#include "DashComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/Widgets/Dash/DashWidget.h"

// Sets default values for this component's properties
UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Jump Component!")))
	{
		MarinePawn = Cast<AMarineCharacter>(GetOwner());
	}
}

void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MoveToDashLocation(DeltaTime);
}

void UDashComponent::Dash()
{
	if (!CanPlayerPerformDash())
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
	const bool bHit = GetCloserHitResult(HitResult);

	// If object that was hit is closer then DashDistance then calculate time for new distance
	CalculatedDashTime = bHit ? (HitResult.Distance * DashTime) / DashDistance : DashTime;

	const FVector& EndRaycastLocation = InitialPlayerPosition + (CalculateDashDirection() * DashDistance);
	return (bHit ? HitResult.Location : EndRaycastLocation) - CalculateDashDirection() * OffsetFromObstacle;
}

bool UDashComponent::GetCloserHitResult(FHitResult& OutHitResult)
{
	const FVector& EndRaycastLowerLoc = InitialPlayerPosition + (CalculateDashDirection() * DashDistance);
	const FVector& EndRaycastHigherLoc = MarinePawn->GetCameraLocation() + (CalculateDashDirection() * DashDistance);

	// if there is obstacle on the way then set actor location to Hit Location with offset
	FHitResult HitResultLower, HitResultHigher;
	const bool bHitLower = GetWorld()->LineTraceSingleByChannel(HitResultLower, InitialPlayerPosition, EndRaycastLowerLoc, ECC_GameTraceChannel7);
	const bool bHitHigher = GetWorld()->LineTraceSingleByChannel(HitResultHigher, MarinePawn->GetCameraLocation(), EndRaycastHigherLoc, ECC_GameTraceChannel7);

	// Returns HitResult that was closer to the player 
	if (bHitLower || bHitHigher)
	{
		if (bHitLower && bHitHigher)
		{
			OutHitResult = HitResultLower.Distance <= HitResultHigher.Distance ? HitResultLower : HitResultHigher;
		}
		OutHitResult = bHitLower ? HitResultLower : HitResultHigher;
		OutHitResult.Location.Z = InitialPlayerPosition.Z;
		return true;
	}

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
	if (IsValid(DashSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), DashSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Dash Sound is nullptr in Dash Component!"));

	if (!IsValid(DashWidgetClass))
		return;

	TObjectPtr<UDashWidget> DashWidget = Cast<UDashWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(),0), DashWidgetClass));
	if (IsValid(DashWidget))
		DashWidget->AddToViewport();

	if (!IsValid(MarinePawn))
		return;
	
	FPowerUpLoaded DashDelay = FPowerUpLoaded(true, DashCoolDown, MarinePawn->GetHudWidget()->ActiveDashAnim, MarinePawn->GetHudWidget()->DashBar);
	MarinePawn->GetHudWidget()->AddNewPowerUpToStartLoading(DashDelay);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_Dash);
}

void UDashComponent::MoveToDashLocation(float Delta)
{
	if (!bIsPerformingDash)
		return;

	if (DashTimeElapsed <= CalculatedDashTime)
	{
		const FVector& NewLoc = FMath::Lerp(InitialPlayerPosition, DashLocation, DashTimeElapsed / CalculatedDashTime);
		MarinePawn->SetActorLocation(NewLoc);
	}
	else
	{
		TurnOffDash();
	}

	DashTimeElapsed += (Delta / UGameplayStatics::GetGlobalTimeDilation(GetWorld())); // When in slowmotion the speed of dash is the same
}

void UDashComponent::TurnOffDash()
{
	DashTimeElapsed = 0.f;
	bIsPerformingDash = false;
	GetWorld()->GetTimerManager().SetTimer(DashCooldownHandle, this, &UDashComponent::EndDashCooldown, DashCoolDown, false);
}

bool UDashComponent::CanPlayerPerformDash() const
{
	if (!IsValid(MarinePawn)|| !bCanDash)
		return false;

	if (MarinePawn->GetIsPlayerMovingToHookLocation()|| MarinePawn->GetIsWallrunning()) 
		return false;

	if (MarinePawn->GetIsCrouching() || MarinePawn->GetIsInCutscene())
		return false;

	if (MarinePawn->GetInputAxisValue("Right") == 0.f && MarinePawn->GetInputAxisValue("Forward") == 0.f)
		return false;

	return true;
}