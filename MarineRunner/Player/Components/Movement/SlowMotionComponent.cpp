// Copyright Adam Bartela.All Rights Reserved

#include "SlowMotionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"

// Sets default values for this component's properties
USlowMotionComponent::USlowMotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void USlowMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Slow Motion Component!")))
	{
		MarinePawn = Cast<AMarineCharacter>(GetOwner());
	}
	MarinePawn->OnDestroyed.AddDynamic(this, &USlowMotionComponent::OnOwnerDestroyed);
}

void USlowMotionComponent::OnOwnerDestroyed(AActor* DestroyedActor)
{
	PauseSlowMotionSound(true);
}

void USlowMotionComponent::SlowMotionPressed()
{
	if (!IsValid(MarinePawn))
		return;

	if (MarinePawn->GetIsPlayerMovingToHookLocation() || MarinePawn->GetIsInCutscene()) 
		return;
	
	SuddentDisabledSlowMotion();

	if (!bCanSlowMotion) 
		return;

	SettingUpSlowMotion();

	GetWorld()->GetTimerManager().SetTimer(SlowMotionTimeHandle, this, &USlowMotionComponent::DisableSlowMotion, SlowMotionTime * SlowMotionValue);
}	

void USlowMotionComponent::SettingUpSlowMotion()
{
	bCanSlowMotion = false;
	bIsInSlowMotion = true;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), SlowMotionValue);
	
	SlowMotionEffects();
}

void USlowMotionComponent::SuddentDisabledSlowMotion()
{
	if (!bIsInSlowMotion)
		return;

	if (IsValid(SlowMotionSoundSpawned))
		SlowMotionSoundSpawned->Stop();

	if (CancelSlowMotionSound)
		UGameplayStatics::PlaySound2D(GetWorld(), CancelSlowMotionSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Cancel Slow Motion Sound is nullptr in SlowMotionComponent!"));

	GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimeHandle);
	DisableSlowMotion();
}

void USlowMotionComponent::SlowMotionEffects()
{
	if (IsValid(SlowMotionSound)) 
		SlowMotionSoundSpawned = UGameplayStatics::SpawnSound2D(GetWorld(), SlowMotionSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Slow Motion Sound is nullptr in SlowMotionComponent!"));

	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), GlobalPitchModulation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	if (!IsValid(MarinePawn))
		return;

	FPowerUpLoaded SlowMoDelay = FPowerUpLoaded(true, SlowMotionTime*SlowMotionValue, MarinePawn->GetHudWidget()->ActiveSlowMotionAnim, MarinePawn->GetHudWidget()->SlowMoBar);
	MarinePawn->GetHudWidget()->AddNewPowerUpToStartLoading(SlowMoDelay);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_SlowMo);
	MarinePawn->GetHudWidget()->SetDeltaDivider(1.f / SlowMotionValue);

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
	const float& NormalTimeSpeed = 1.f;
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), NormalTimeSpeed, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), NormalTimeSpeed);
	MarinePawn->GetHudWidget()->SetDeltaDivider(NormalTimeSpeed);

	MarinePawn->GetCamera()->PostProcessSettings.bOverride_ColorGain = false;
	MarinePawn->GetCamera()->PostProcessSettings.ChromaticAberrationStartOffset = 1.f;
	MarinePawn->GetCamera()->PostProcessSettings.SceneFringeIntensity = 0.f;
	MarinePawn->GetHudWidget()->SetColorAndOpacity(FLinearColor::White);

	FPowerUpLoaded SlowMoDelay = FPowerUpLoaded(true, SlowMotionDelay, MarinePawn->GetHudWidget()->ActiveSlowMotionAnim, MarinePawn->GetHudWidget()->SlowMoBar);
	MarinePawn->GetHudWidget()->AddNewPowerUpToStartLoading(SlowMoDelay);
	MarinePawn->GetHudWidget()->PlayButtonAnimation(EATP_PressedButton_SlowMo);

	SlowMotionSoundSpawned = nullptr;

	//Enable delay for SlowMotion
	GetWorld()->GetTimerManager().SetTimer(SlowMotionDelayHandle, this, &USlowMotionComponent::SetCanSlowMotionAgain, SlowMotionDelay);
}

void USlowMotionComponent::SetCanSlowMotionAgain()
{
	bCanSlowMotion = true;
}

void USlowMotionComponent::PauseSlowMotionSound(bool bPause)
{
	if (!SlowMotionSoundSpawned)
		return;

	if (!SlowMotionSoundSpawned->IsActive())
		return;

	SlowMotionSoundSpawned->SetPaused(bPause);
}