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
	if (IsValid(MarinePawn) == false)
		return;
	OriginalForce = MarinePawn->GetMovementForce();
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UDashComponent::Dash()
{
	if (CanPlayerPerformDash() == false)
		return;

	MarinePawn->SetShouldAddCounterMovement(true);

	DashOnRamp();

	bCanDash = false;

	if (DashSound) UGameplayStatics::SpawnSound2D(GetWorld(), DashSound);

	MakeDashWidget(true, DashWidgetTime);

	ElementBar DashElementBar{ DashCoolDown };
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::Dash, DashElementBar);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_Dash);

	MarinePawn->GetWorldTimerManager().SetTimer(DashLengthHandle, this, &UDashComponent::DashLengthTimer, DashLength, false);
}

void UDashComponent::DashOnRamp()
{
	if (MarinePawn->GetIsOnRamp() == false)
	{
		MarinePawn->SetMovementForce(DashForce * 10);
	}
	else if (MarinePawn->GetIsGoingUp() == false)
	{
		FVector Impulse = (-MarinePawn->GetActorUpVector() + MarinePawn->GetActorForwardVector()) * DashForce * 300;
		MarinePawn->CapsulePawn->AddImpulse(Impulse);
	}
}

void UDashComponent::MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov, bool bAddChromaticAbberation)
{
	if (!DashWidgetClass || !bShouldMake) 
		return;

	DashWidget = Cast<UDashWidget>(CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), DashWidgetClass));

	DashWidget->SetFadeTime(FadeTime);
	DashWidget->ShouldAddChangingFov(bAddFov);
	DashWidget->ShouldAddChromaticAbberation(bAddChromaticAbberation);

	DashWidget->AddToViewport();
}

void UDashComponent::RemoveDashWidget()
{
	if (IsValid(DashWidget) == false) 
		return;

	DashWidget->ResetDashWidget();
}

void UDashComponent::DashLengthTimer()
{
	MarinePawn->SetMovementForce(OriginalForce);
	MarinePawn->GetWorldTimerManager().SetTimer(DashCooldownHandle, this, &UDashComponent::EndDashCooldown, DashCoolDown, false);
}

bool UDashComponent::CanPlayerPerformDash() const
{
	if (IsValid(MarinePawn) == false || bCanDash == false)
		return false;

	if (MarinePawn->GetIsPlayerLerpingToHookLocation() == true || MarinePawn->GetIsWallrunning() == true) 
		return false;

	if (MarinePawn->GetIsInSlowMotion() == true || MarinePawn->GetIsCrouching() == true)
		return false;

	return true;
}
