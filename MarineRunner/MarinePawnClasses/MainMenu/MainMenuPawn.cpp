// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/MarinePawnClasses/MainMenu/MainMenuWidget.h"

// Sets default values
AMainMenuPawn::AMainMenuPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnMainMenuWidget();
}

// Called every frame
void AMainMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("MainMenu"), IE_Pressed, this, &AMainMenuPawn::BackToPreviousMenu);
}

void AMainMenuPawn::SpawnMainMenuWidget()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (IsValid(PC) == false)
		return;

	PC->SetShowMouseCursor(true);

	MainMenuWidget = Cast<UMainMenuWidget>(CreateWidget(PC, MainMenuWidgetClass));
	if (IsValid(MainMenuWidget) == false)
		return;

	MainMenuWidget->AddToViewport();
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PC, MainMenuWidget);

	bIsInMainMenu = true;
}

void AMainMenuPawn::BackToPreviousMenu()
{
	if (bIsInMainMenu == false || IsValid(MainMenuWidget) == false)
		return;

	MainMenuWidget->RemoveCurrentMenuWidgetsFromViewport();
}

