// Copyright Adam Bartela.All Rights Reserved

#include "SlowMotionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Widgets/HUDWidget.h"

// Sets default values for this component's properties
USlowMotionComponent::USlowMotionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void USlowMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	if (IsValid(MarinePawn) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("MARINE PAWN IS NOT SET IN SLOW MOTIOn COMPONENT!"));
		return;
	}

	MarinePawn->OnDestroyed.AddDynamic(this, &USlowMotionComponent::OnOwnerDestroyed);
}

void USlowMotionComponent::OnOwnerDestroyed(AActor* DestroyedActor)
{
	PauseSlowMotionSound(true);
}

void USlowMotionComponent::SlowMotionPressed()
{
	if (MarinePawn->GetIsPlayerLerpingToHookLocation() == true) 
		return;
	
	SuddentDisabledSlowMotion();

	if (bCanSlowMotion == false) 
		return;

	SettingUpSlowMotion();

	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimeHandle, this, &USlowMotionComponent::DisableSlowMotion, SlowMotionTime * SlowMotionValue);
}	

void USlowMotionComponent::SettingUpSlowMotion()
{
	bCanSlowMotion = false;
	bIsInSlowMotion = true;

	//SlowMotion command
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionValue);

	SlowMotionEffects();
}

void USlowMotionComponent::SuddentDisabledSlowMotion()
{
	if (bIsInSlowMotion == false)
		return;

	if (IsValid(SlowMotionSoundSpawned))
		SlowMotionSoundSpawned->Stop();
	if (CancelSlowMotionSound)
		UGameplayStatics::PlaySound2D(GetWorld(), CancelSlowMotionSound);

	GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimeHandle);
	DisableSlowMotion();
}

void USlowMotionComponent::SlowMotionEffects()
{
	if (SlowMotionSound) SlowMotionSoundSpawned = UGameplayStatics::SpawnSound2D(GetWorld(), SlowMotionSound);

	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 0.5f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	if (IsValid(MarinePawn) == false)
		return;

	ElementBar SlowMoElementBar{ SlowMotionTime };
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::SlowMo, SlowMoElementBar);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_SlowMo);

	MarinePawn->GetCamera()->PostProcessSettings.bOverride_ColorGain = true;
	MarinePawn->GetCamera()->PostProcessSettings.ColorGain = ScreenColorWhenInSlowMotion;

	MarinePawn->GetCamera()->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	MarinePawn->GetCamera()->PostProcessSettings.bOverride_ChromaticAberrationStartOffset = true;

	MarinePawn->GetCamera()->PostProcessSettings.ChromaticAberrationStartOffset = 0.f;
	MarinePawn->GetCamera()->PostProcessSettings.SceneFringeIntensity = StartingChromaticAbberation;

	MarinePawn->GetHudWidget()->SetColorAndOpacity(ScreenColorWhenInSlowMotion);
}

void USlowMotionComponent::DisableSlowMotion()
{
	bIsInSlowMotion = false;
	const float NormalTimeSpeed = 1.f;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), NormalTimeSpeed, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NormalTimeSpeed);

	MarinePawn->GetCamera()->PostProcessSettings.bOverride_ColorGain = false;
	MarinePawn->GetCamera()->PostProcessSettings.ChromaticAberrationStartOffset = 1.f;
	MarinePawn->GetCamera()->PostProcessSettings.SceneFringeIntensity = 0.f;
	MarinePawn->GetHudWidget()->SetColorAndOpacity(FLinearColor::White);

	ElementBar SlowMoElementBar{ SlowMotionDelay };
	MarinePawn->GetHudWidget()->AddElementToProgress(EUseableElement::SlowMo, SlowMoElementBar);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_SlowMo);

	SlowMotionSoundSpawned = nullptr;

	//Enable delay for SlowMotion
	GetWorld()->GetTimerManager().SetTimer(SlowMotionDelayHandle, this, &USlowMotionComponent::DelayCompleted, SlowMotionDelay);
}

void USlowMotionComponent::DelayCompleted()
{
	bCanSlowMotion = true;
}

void USlowMotionComponent::PauseSlowMotionSound(bool bPause)
{
	if (SlowMotionSoundSpawned == nullptr)
		return;

	if (SlowMotionSoundSpawned->IsActive() == false)
		return;

	SlowMotionSoundSpawned->SetPaused(bPause);
}