// Copyright Adam Bartela.All Rights Reserved


#include "MessageHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/Interactive/MessageToRead/MessageToReadWidget.h"
#include "MarineRunner/Player/MarinePlayer.h"

// Sets default values for this component's properties
UMessageHandlerComponent::UMessageHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMessageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

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

	const TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

void UMessageHandlerComponent::SpawnWidget(const TSubclassOf<UUserWidget>& WidgetClassToSpawn)
{
	if (!IsValid(WidgetClassToSpawn))
		return;

	TObjectPtr<UUserWidget> NewWidget = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetClassToSpawn);
	if (!IsValid(NewWidget))
		return;

	if (MessagePopUpSound)
		UGameplayStatics::PlaySound2D(GetWorld(), MessagePopUpSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Message Pop up sound is nullptr in Message Handler component!"));

	NewWidget->AddToViewport();
}

