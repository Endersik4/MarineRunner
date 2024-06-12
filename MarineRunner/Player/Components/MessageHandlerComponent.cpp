// Copyright Adam Bartela.All Rights Reserved


#include "MessageHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/Interactive/MessageToRead/MessageToReadWidget.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Framework/Cheats/CheatWidget.h"

// Sets default values for this component's properties
UMessageHandlerComponent::UMessageHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMessageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player controller is nullptr in MessageHandlerComponent!")))
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
}

void UMessageHandlerComponent::DeleteCurrentDisplayedMessage(AMarineCharacter* Player)
{
	if (!IsValid(Player))
		return;

	if (Player->GetIsInPauseMenu())
		return;

	if (!IsValid(CurrentDisplayedMessage) || !bIsMessageDisplayed)
		return;

	CurrentDisplayedMessage->RemoveFromParent();
	bIsMessageDisplayed = false;
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	if (!IsValid(PlayerController))
		return;

	PlayerController->SetShowMouseCursor(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}

void UMessageHandlerComponent::SpawnNewRecipeUnlockedWidget()
{
	SpawnWidget(NewRecipeUnlockedClassWidget);
}

void UMessageHandlerComponent::SpawnNotEnoughSlotsForWeaponWidget()
{
	SpawnWidget(NotEnoughSlotsForWeaponClassWidget);
}

void UMessageHandlerComponent::SpawnCheatsWidget()
{
	if (!IsValid(PlayerController))
		return;

	if (bIsCheatsDisplayed && IsValid(SpawnedCheatWidget))
	{
		SpawnedCheatWidget->RemoveFromParent();
		PlayerController->SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		SpawnedCheatWidget = nullptr;
		bIsCheatsDisplayed = false;
		return;
	}

	bIsCheatsDisplayed = true;
	SpawnedCheatWidget = SpawnWidget(CheatWidgetClass);

	PlayerController->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController);
}

TObjectPtr<class UUserWidget> UMessageHandlerComponent::SpawnWidget(const TSubclassOf<UUserWidget>& WidgetClassToSpawn)
{
	if (!IsValid(WidgetClassToSpawn))
		return nullptr;

	TObjectPtr<UUserWidget> NewWidget = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetClassToSpawn);
	if (!IsValid(NewWidget))
		return nullptr;

	if (MessagePopUpSound)
		UGameplayStatics::PlaySound2D(GetWorld(), MessagePopUpSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Message Pop up sound is nullptr in Message Handler component!"));

	NewWidget->AddToViewport();
	return NewWidget;
}

