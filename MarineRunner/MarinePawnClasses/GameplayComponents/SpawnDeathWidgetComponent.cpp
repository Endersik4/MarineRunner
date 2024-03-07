// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SpawnDeathWidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/PlayerWidgets/YouDiedWidget.h"

USpawnDeathWidgetComponent::USpawnDeathWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpawnDeathWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USpawnDeathWidgetComponent::SpawnDeathWidget(TObjectPtr<APlayerController> PlayerController)
{
	if (!IsValid(YouDiedWidgetClass) || !IsValid(PlayerController))
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
		return;
	}

	YouDiedWidget = Cast<UYouDiedWidget>(CreateWidget(PlayerController, YouDiedWidgetClass));
	if (!IsValid(YouDiedWidget))
		return;

	YouDiedWidget->AddToViewport();
	PlayerController->SetShowMouseCursor(true);

	GetWorld()->GetTimerManager().SetTimer(PauseGameHandle, this, &USpawnDeathWidgetComponent::PauseGameAfterDelay, DelayForGamePause, false);
}

void USpawnDeathWidgetComponent::PauseGameAfterDelay()
{
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void USpawnDeathWidgetComponent::RestartGameInYouDiedWidget()
{
	if (!IsValid(YouDiedWidget))
		return;

	YouDiedWidget->RestartGameButton_OnClicked();
}

void USpawnDeathWidgetComponent::QuitGameInYouDiedWidget()
{
	if (!IsValid(YouDiedWidget))
		return;

	YouDiedWidget->QuitButton_OnClicked();
}



