// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowMotionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "DashComponent.h"

// Sets default values for this component's properties
USlowMotionComponent::USlowMotionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void USlowMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (IsValid(MarinePawn) == false) 
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));
}

// Called every frame
void USlowMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void USlowMotionComponent::SlowMotionPressed()
{
	if (MarinePawn->GetIsPlayerLerpingToHookLocation() == true) 
		return;
	
	SuddentDisabledSlowMotion();

	if (bCanSlowMotion == false) return;
	SettingUpSlowMotion();

	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimeHandle, this, &USlowMotionComponent::DisableSlowMotion, SlowMotionValue * SlowMotionTime);
}	

void USlowMotionComponent::SuddentDisabledSlowMotion()
{
	if (bIsInSlowMotion == false)
		return;

	if (IsValid(SlowMotionSoundSpawned)) SlowMotionSoundSpawned->Stop();
	if (CancelSlowMotionSound) UGameplayStatics::PlaySound2D(GetWorld(), CancelSlowMotionSound);
	MarinePawn->GetDashComponent()->RemoveDashWidget();

	GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimeHandle);
	DisableSlowMotion();
}

void USlowMotionComponent::SettingUpSlowMotion()
{
	bCanSlowMotion = false;
	bIsInSlowMotion = true;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 0.5f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	if (SlowMotionSound) SlowMotionSoundSpawned = UGameplayStatics::SpawnSound2D(GetWorld(), SlowMotionSound);

	//SlowMotion command
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionValue);
	MarinePawn->CustomTimeDilation = SlowMotionValue;
	
	//DashWidget will be on player viewport for some time (SlowMotionTime)
	MarinePawn->GetDashComponent()->MakeDashWidget(true, SlowMotionValue * SlowMotionTime, false);

	ElementBar SlowMoElementBar{ SlowMotionDelay};
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::SlowMo, SlowMoElementBar);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_SlowMo);
}

void USlowMotionComponent::DisableSlowMotion()
{
	bIsInSlowMotion = false;
	const float NormalTimeSpeed = 1.f;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), NormalTimeSpeed, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NormalTimeSpeed);
	MarinePawn->CustomTimeDilation = NormalTimeSpeed;

	//Enable delay for SlowMotion
	GetWorld()->GetTimerManager().SetTimer(SlowMotionDelayHandle, this, &USlowMotionComponent::DelayCompleted, SlowMotionDelay);
}

void USlowMotionComponent::DelayCompleted()
{
	bCanSlowMotion = true;

}

