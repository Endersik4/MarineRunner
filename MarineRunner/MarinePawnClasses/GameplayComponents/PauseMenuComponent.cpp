// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/PauseMenuComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/Widgets/Menu/PauseMenuWidget.h"


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
}


// Called every frame
void UPauseMenuComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPauseMenuComponent::PauseGame()
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) == true)
	{
		UnPauseGame();
		return;
	}

	if (IsValid(PlayerController) == false)
		return;

	PlayerController->SetShowMouseCursor(true);
	SpawnPauseMenuWidget();

	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void UPauseMenuComponent::UnPauseGame()
{
	bool bRemovePauseMenuWidget = PauseMenuWidget->RemoveCurrentMenuWidgetsFromViewport();

	if (bRemovePauseMenuWidget == false || IsValid(PlayerController) == false) 
		return;

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	PlayerController->SetShowMouseCursor(false);

	PauseMenuWidget->RemoveFromParent();
	PauseMenuWidget = nullptr;

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
}

void UPauseMenuComponent::SpawnPauseMenuWidget()
{
	if (IsValid(PlayerController) == false) 
		return;

	PauseMenuWidget = Cast<UPauseMenuWidget>(CreateWidget(PlayerController, PauseMenuWidgetClass));
	if (IsValid(PauseMenuWidget) == false)
		return;

	PauseMenuWidget->AddToViewport();
}
