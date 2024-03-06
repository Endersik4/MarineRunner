// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuWidget.h"

AMainMenuPawn::AMainMenuPawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnMainMenuWidget();
}

void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, this, &AMainMenuPawn::BackToPreviousMenu);
}

void AMainMenuPawn::SpawnMainMenuWidget()
{
	TObjectPtr<APlayerController> PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!IsValid(PlayerController) || !IsValid(MainMenuWidgetClass))
		return;

	PlayerController->SetShowMouseCursor(true);

	MainMenuWidget = Cast<UMainMenuWidget>(CreateWidget(PlayerController, MainMenuWidgetClass));
	if (!IsValid(MainMenuWidget))
		return;

	MainMenuWidget->AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, MainMenuWidget);

	bIsInMainMenu = true;
}

void AMainMenuPawn::BackToPreviousMenu()
{
	if (!bIsInMainMenu || !IsValid(MainMenuWidget))
		return;

	MainMenuWidget->RemoveCurrentMenuWidgetsFromViewport();
}
