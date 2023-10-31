// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/MessageHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values for this component's properties
UMessageHandlerComponent::UMessageHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UMessageHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UMessageHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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

