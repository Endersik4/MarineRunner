// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/SlowMotionComponent.h"

// Sets default values for this component's properties
UPauseMenuComponent::UPauseMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPauseMenuComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}

void UPauseMenuComponent::PauseGame()
{
	if (CanPauseGame() == false)
		return;

	PlayerController->SetShowMouseCursor(true);
	bIsInPauseMenu = true;
	SpawnPauseMenuWidget();
	MarinePawn->GetSlowMotionComponent()->PauseSlowMotionSound(true);

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void UPauseMenuComponent::UnPauseGame()
{
	bool bRemovePauseMenuWidget = PauseMenuWidget->RemoveCurrentMenuWidgetsFromViewport();

	if (bRemovePauseMenuWidget == false || IsValid(PlayerController) == false)
		return;

	if (MarinePawn->GetIsMessageDisplayed() == false)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		ChangeUIToGameOnly();
	}
	bIsInPauseMenu = false;

	MarinePawn->GetSlowMotionComponent()->PauseSlowMotionSound(false);

	PauseMenuWidget->RemoveFromParent();
	PauseMenuWidget = nullptr;
}

void UPauseMenuComponent::SpawnPauseMenuWidget()
{
	if (IsValid(PlayerController) == false)
		return;

	PauseMenuWidget = Cast<UPauseMenuWidget>(CreateWidget(PlayerController, PauseMenuWidgetClass));
	if (IsValid(PauseMenuWidget) == false)
		return;

	PauseMenuWidget->AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, PauseMenuWidget);
}

bool UPauseMenuComponent::CanPauseGame()
{
	if (IsValid(MarinePawn) == false)
		return false;

	if (MarinePawn->GetIsDead() == true)
		return false;

	if (bIsInPauseMenu == true)
	{
		UnPauseGame();
		return false;
	}

	if (IsValid(PlayerController) == false)
		return false;

	return true;
}

void UPauseMenuComponent::ChangeUIToGameOnly()
{
	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}