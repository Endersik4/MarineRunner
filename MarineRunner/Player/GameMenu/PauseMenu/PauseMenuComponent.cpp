// Copyright Adam Bartela.All Rights Reserved


#include "PauseMenuComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "PauseMenuWidget.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/Movement/SlowMotionComponent.h"
#include "MarineRunner/Player/Components/MessageHandlerComponent.h"

// Sets default values for this component's properties
UPauseMenuComponent::UPauseMenuComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPauseMenuComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(UGameplayStatics::GetPlayerController(GetWorld(), 0), TEXT("Player Controller is nullptr in PauseMenuComponent!")))
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
	if (ensureMsgf(GetOwner(), TEXT("Player is nullptr in PauseMenuComponent!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}
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
	const bool bRemovePauseMenuWidget = PauseMenuWidget->BackToPreviousMenu();

	if (!bRemovePauseMenuWidget|| !IsValid(PlayerController))
		return;

	bIsInPauseMenu = false;

	Player->GetSlowMotionComponent()->PauseSlowMotionSound(false);

	PauseMenuWidget->RemoveFromParent();
	PauseMenuWidget = nullptr;

	if (Player->GetMessageHandlerComponent()->GetIsMessageDisplayed())
		return;

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (Player->GetMessageHandlerComponent()->GetIsCheatsDisplayed())
		return;

	ChangeUIToGameOnly();
}

void UPauseMenuComponent::SpawnPauseMenuWidget()
{
	if (!IsValid(PlayerController) || !IsValid(PauseMenuWidgetClass))
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
	if (!IsValid(PlayerController))
		return;

	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}