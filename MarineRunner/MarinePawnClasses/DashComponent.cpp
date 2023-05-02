// Fill out your copyright notice in the Description page of Project Settings.


#include "DashComponent.h"
#include "TimerManager.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"

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
	if (MarinePawn)
	{
		CopyForce = MarinePawn->GetForce();
	}
}


// Called every frame
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

}

void UDashComponent::Dash()
{
	if (MarinePawn == nullptr) return;
	if (bCanDash)
	{
		if (!MarinePawn->GetIsOnRamp()) MarinePawn->SetForce(DashForce*10);
		else if(!MarinePawn->GetIsGoingUp())
		{
			FVector Impulse = (-MarinePawn->GetActorUpVector() + MarinePawn->GetActorForwardVector()) * DashForce * 300;
			MarinePawn->CapsulePawn->AddImpulse(Impulse);
		}
		bCanDash = false;

		MarinePawn->MakeDashWidget(true, DashWidgetTime);

		ElementBar DashElementBar{ DashCoolDown }, ButtonDashElementBar{0.3f};
		MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::Dash, DashElementBar);
		MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::Button_Dash, ButtonDashElementBar);

		MarinePawn->GetWorldTimerManager().SetTimer(DashLengthHandle, this, &UDashComponent::DashLengthTimer, DashLength, false);
	}
}

void UDashComponent::DashLengthTimer()
{
	MarinePawn->SetForce(CopyForce);
	UE_LOG(LogTemp, Warning, TEXT("NIEEE"));
	MarinePawn->GetWorldTimerManager().SetTimer(DashCooldownHandle, this, &UDashComponent::DashCooldownTimer, DashCoolDown, false);
}

void UDashComponent::DashCooldownTimer()
{
	UE_LOG(LogTemp, Warning, TEXT("MONZA JUZ"));
	bCanDash = true;
}
