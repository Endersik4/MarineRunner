// Copyright Adam Bartela.All Rights Reserved

#include "DashComponent.h"
#include "TimerManager.h"
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
	bCanDash = true;

	InitialPlayerPosition = MarinePawn->GetActorLocation();
	DashLocation = CalculateEndDashPosition();

	DashEffects();
}

FVector UDashComponent::CalculateEndDashPosition()
{
	FVector EndRaycastLoc = InitialPlayerPosition + (CalculateDashDirection() * DashDistance);

	// if there is obstacle on the way then set actor location to Hit Location with offset
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, InitialPlayerPosition, EndRaycastLoc, ECollisionChannel::ECC_Visibility);

	return (bHit ? HitResult.Location : EndRaycastLoc) - CalculateDashDirection() * OffsetFromObstacle;
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

	if (DashTimeElapsed <= DashTime)
	{
		FVector NewLoc = FMath::Lerp(InitialPlayerPosition, DashLocation, DashTimeElapsed / DashTime);
		MarinePawn->SetActorLocation(NewLoc);
	}
	else
	{
		TurnOffDash();
	}
	DashTimeElapsed += Delta;
}

void UDashComponent::TurnOffDash()
{
	DashTimeElapsed = 0.f;
	bIsPerformingDash = false;
	MarinePawn->GetWorldTimerManager().SetTimer(DashCooldownHandle, this, &UDashComponent::EndDashCooldown, DashCoolDown, false);
}

bool UDashComponent::CanPlayerPerformDash() const
{
	if (IsValid(MarinePawn) == false || bCanDash == false)
		return false;

	if (MarinePawn->GetIsPlayerLerpingToHookLocation() == true || MarinePawn->GetIsWallrunning() == true) 
		return false;

	if (MarinePawn->GetIsCrouching() == true)
		return false;

	return true;
}