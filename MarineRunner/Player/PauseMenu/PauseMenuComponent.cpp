// Copyright Adam Bartela.All Rights Reserved


#include "PauseMenuComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "PauseMenuWidget.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/Movement/SlowMotionComponent.h"

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
	Player = Cast<AMarineCharacter>(GetOwner());
}

void UPauseMenuComponent::PauseGame()
{
	if (!CanPauseGame())
		return;

	PlayerController->SetShowMouseCursor(true);
	bIsInPauseMenu = true;
	SpawnPauseMenuWidget();
	Player->GetSlowMotionComponent()->PauseSlowMotionSound(true);

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void UPauseMenuComponent::UnPauseGame()
{
	bool bRemovePauseMenuWidget = PauseMenuWidget->RemoveCurrentMenuWidgetsFromViewport();

	if (!bRemovePauseMenuWidget|| !IsValid(PlayerController))
		return;

	if (!Player->GetIsMessageDisplayed())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		ChangeUIToGameOnly();
	}
	bIsInPauseMenu = false;

	Player->GetSlowMotionComponent()->PauseSlowMotionSound(false);

	PauseMenuWidget->RemoveFromParent();
	PauseMenuWidget = nullptr;
}

void UPauseMenuComponent::SpawnPauseMenuWidget()
{
	if (!IsValid(PlayerController))
		return;

	PauseMenuWidget = Cast<UPauseMenuWidget>(CreateWidget(PlayerController, PauseMenuWidgetClass));
	if (!IsValid(PauseMenuWidget))
		return;

	PauseMenuWidget->AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, PauseMenuWidget);
}

bool UPauseMenuComponent::CanPauseGame()
{
	if (!IsValid(Player))
		return false;

	if (Player->GetIsDead())
		return false;

	if (bIsInPauseMenu)
	{
		UnPauseGame();
		return false;
	}

	if (!IsValid(PlayerController))
		return false;

	return true;
}

void UPauseMenuComponent::ChangeUIToGameOnly()
{
	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}