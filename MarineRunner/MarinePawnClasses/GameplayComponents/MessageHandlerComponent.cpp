// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

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

void UMessageHandlerComponent::DeleteCurrentDisplayedMessage(class AMarineCharacter* Player)
{
	if (Player->GetIsInPauseMenu())
		return;

	if (IsValid(CurrentDisplayedMessage) == false || bIsMessageDisplayed == false)
		return;

	CurrentDisplayedMessage->RemoveFromParent();
	bIsMessageDisplayed = false;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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
	if (WidgetClassToSpawn == nullptr)
		return;

	UUserWidget* NewWidget = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), WidgetClassToSpawn);
	if (IsValid(NewWidget) == false)
		return;

	if (MessagePopUpSound)
		UGameplayStatics::PlaySound2D(GetWorld(), MessagePopUpSound);

	NewWidget->AddToViewport();
}

