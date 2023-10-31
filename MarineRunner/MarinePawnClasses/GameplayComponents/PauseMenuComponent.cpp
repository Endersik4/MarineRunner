// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"
#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"


// Sets default values for this component's properties
UPauseMenuComponent::UPauseMenuComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPauseMenuComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}


// Called every frame
void UPauseMenuComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPauseMenuComponent::PauseGame()
{
	if (CanPauseGame() == false)
		return;

	PlayerController->SetShowMouseCursor(true);
	bIsInPauseMenu = true;
	SpawnPauseMenuWidget();

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
		PlayerController->SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	}
	bIsInPauseMenu = false;

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
