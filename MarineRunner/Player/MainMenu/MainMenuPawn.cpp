// Copyright Adam Bartela.All Rights Reserved

#include "MainMenuPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MainMenuWidget.h"
#include "SelectDifficultyWidget.h"

AMainMenuPawn::AMainMenuPawn()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	SpawnMainMenuWidget();
}

void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, this, &AMainMenuPawn::BackToPreviousMenu);

	PlayerInputComponent->BindAction(TEXT("AnyKeyToContinue"), IE_Pressed, this, &AMainMenuPawn::ContinueToSelectDifficulty);
}

void AMainMenuPawn::SpawnMainMenuWidget()
{
	if (!IsValid(PlayerController) || !IsValid(MainMenuWidgetClass))
		return;

	PlayerController->SetShowMouseCursor(true);

	SpawnedMainMenuWidget = Cast<UMainMenuWidget>(CreateWidget(PlayerController, MainMenuWidgetClass));
	if (!IsValid(SpawnedMainMenuWidget))
		return;

	SpawnedMainMenuWidget->AddToViewport();
	SpawnedMainMenuWidget->SetMainMenuPawn(this);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController, SpawnedMainMenuWidget);

	bIsInMainMenu = true;
}

void AMainMenuPawn::BackToPreviousMenu()
{
	if (!bIsInMainMenu || !IsValid(SpawnedMainMenuWidget))
		return;

	SpawnedMainMenuWidget->BackToPreviousMenu();
}

void AMainMenuPawn::ContinueToSelectDifficulty()
{
	if (!bCanPressAnyKeyToContinue)
		return;

	bCanPressAnyKeyToContinue = false;

	SpawnedMainMenuWidget->ShowZTGKInformation(false);

	FTimerHandle SpawnDifficultyWidgetHandle;
	GetWorld()->GetTimerManager().SetTimer(SpawnDifficultyWidgetHandle, this, &AMainMenuPawn::SpawnDifficultyWidget, SpawnDifficultyWidgetTime, false);
}

void AMainMenuPawn::SpawnDifficultyWidget()
{
	if (!IsValid(PlayerController) || !IsValid(SelectDifficultyWidgetClass))
		return;

	TObjectPtr<USelectDifficultyWidget> SpawnedSelectDifficultyWidget = Cast<USelectDifficultyWidget>(CreateWidget(PlayerController, SelectDifficultyWidgetClass));
	if (!IsValid(SpawnedSelectDifficultyWidget))
		return;

	SpawnedSelectDifficultyWidget->AddToViewport();
	SpawnedSelectDifficultyWidget->CallNewGameFunc = [this]() {this->SpawnedMainMenuWidget->StartNewGame(); };
	SpawnedSelectDifficultyWidget->CallMainMenuMusicFadeOutFunc = [this]() {this->SpawnedMainMenuWidget->MainMusicFadeOut(); };
}