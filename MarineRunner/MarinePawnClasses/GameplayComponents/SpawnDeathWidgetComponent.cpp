// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SpawnDeathWidgetComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/PlayerWidgets/YouDiedWidget.h"


// Sets default values for this component's properties
USpawnDeathWidgetComponent::USpawnDeathWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void USpawnDeathWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

}

void USpawnDeathWidgetComponent::SpawnDeathWidget(APlayerController* PlayerController)
{
	if (YouDiedWidgetClass == false || IsValid(PlayerController) == false)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName(*UGameplayStatics::GetCurrentLevelName(GetWorld())));
		return;
	}

	YouDiedWidget = Cast<UYouDiedWidget>(CreateWidget(PlayerController, YouDiedWidgetClass));
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
	if (IsValid(YouDiedWidget) == false)
		return;

	YouDiedWidget->RestartGameButton_OnClicked();
}

void USpawnDeathWidgetComponent::QuitGameInYouDiedWidget()
{
	if (IsValid(YouDiedWidget) == false)
		return;

	YouDiedWidget->QuitButton_OnClicked();
}



