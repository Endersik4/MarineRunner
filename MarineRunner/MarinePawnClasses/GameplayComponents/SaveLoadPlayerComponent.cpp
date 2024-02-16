// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"

// Sets default values for this component's properties
USaveLoadPlayerComponent::USaveLoadPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveLoadPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());
	PlayerController = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	LoadSavedSettingsFromGameInstance();

	LoadGame();
}

void USaveLoadPlayerComponent::SpawnNewGameWidget()
{
	if (GameInstance->bNewGame == false)
		return;
	
	SpawnPassingWidget(NewGameBeginsWidgetClass);
	GameInstance->bNewGame = false;

	if (IsValid(Player->GetHudWidget()))
		Player->GetHudWidget()->NewGameStartedWidgetAnimation();
}

void USaveLoadPlayerComponent::SaveGame(const FString& _SaveName, const FString& _WildCard)
{
	if (IsValid(Player) == false)
		return;

	CreatedSaveGame = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));

	CreatedSaveGame->PrepareSaveGame(_SaveName);
	CreatedSaveGame->CopySaveInfoToCurrentGameInstance(GetWorld(), _WildCard);
	
	CreatedSaveGame->CurrentHealthSaved = Player->GetHealth();
	CreatedSaveGame->SavedPlayerLocation = Player->GetActorLocation();
	CreatedSaveGame->SavedPlayerRotation = Player->GetActorRotation();

	CreatedSaveGame->Inventory_ItemsSaved = Player->GetInventoryComponent()->Inventory_Items;
	CreatedSaveGame->Inventory_RecipesSaved = Player->GetInventoryComponent()->Items_Recipes;

	Player->GetWeaponInventoryComponent()->SaveInitialWeaponInventory();
	CreatedSaveGame->WeaponInventory_Saved = Player->GetWeaponInventoryComponent()->InitialWeaponInventory;

	SavedDataObject->EmptyTempCustomSavedData();
	SavedDataObject->UpdateObjectsData();
	CreatedSaveGame->SavedCustomData = SavedDataObject->GetCustomSavedData();

	CreatedSaveGame->MakeJsonFileWithSaveInfo(PlayerController, UGameplayStatics::GetCurrentLevelName(GetWorld()));

	CreatedSaveGame->SavedExplorationMusic = GameInstance->GetCurrentExplorationMusic();

	UGameplayStatics::SaveGameToSlot(CreatedSaveGame, CreatedSaveGame->GetSaveGameName() + "/" + CreatedSaveGame->GetSaveGameName(), 0);

	SpawnPassingWidget(GameSavedNotificationWidgetClass);
}

void USaveLoadPlayerComponent::LoadGame()
{
	if (IsValid(GameInstance) == false || IsValid(Player) == false)
		return;

	if (GameInstance->bNewGame == true)
		return;

	FString SlotName = GameInstance->SlotSaveGameNameToLoad;
	SlotName += "/" + SlotName;

	USaveMarineRunner* LoadGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	if (!UGameplayStatics::LoadGameFromSlot(SlotName, 0))
	{
		return;
	}

	LoadGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	Player->SetActorLocation(LoadGameInstance->SavedPlayerLocation);
	if (IsValid(PlayerController) == true)
		PlayerController->SetControlRotation(LoadGameInstance->SavedPlayerRotation);

	LoadGameInstance->LoadGame(Player, GameInstance);
	LoadGameInstance->LoadOtherObjectsData(SavedDataObject);
}

void USaveLoadPlayerComponent::RestartGame()
{
	if (IsValid(Player) == false)
		return;

	TArray<AActor*> AllEnemiesOnMap;
	auto lambda = [&AllEnemiesOnMap]() {
		for (AActor* CurrentEnemy : AllEnemiesOnMap)
		{
			CurrentEnemy->Destroy();
		}
	};

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGun::StaticClass(), AllEnemiesOnMap);
	lambda();

	SavedDataObject->RestartObjectsData();

	AMarineCharacter* SpawnedNewPlayer = GetWorld()->SpawnActor<AMarineCharacter>(PlayerClass, FTransform(FRotator(0.f), FVector(0.f)));
	if (IsValid(SpawnedNewPlayer) == false)
		return;

	SpawnedNewPlayer->SetAlbertosPawn(Player->GetAlbertosPawn());
	SpawnedNewPlayer->GetSaveLoadPlayerComponent()->SetSavedDataObject(Player->GetSaveLoadPlayerComponent()->GetSavedDataObject());
	Player->GetAlbertosPawn()->GetPlayerIsNearComponent()->SetPlayerPawn(SpawnedNewPlayer);
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->Possess(SpawnedNewPlayer);

	GameInstance->ResetDetectedEnemy();

	Player->Destroy();
}

void USaveLoadPlayerComponent::LoadSavedSettingsFromGameInstance()
{
	if (IsValid(GameInstance) == false || IsValid(PlayerController) == false)
		return;

	Player->LoadFieldOfViewFromSettings();

	const FSettingSavedInJsonFile& CurrentMouseSensName = PlayerController->GetMouseSensitivity();
	FSettingSavedInJsonFile MouseSensitivityJson = Player->GetMouseSensitivityJSON();

	GameInstance->FindSavedValueAccordingToName(MouseSensitivityJson.FieldName, MouseSensitivityJson.FieldValue);
	if (CurrentMouseSensName == MouseSensitivityJson) Player->ChangeMouseSensitivity(MouseSensitivityJson);
}

void USaveLoadPlayerComponent::SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn)
{
	if (IsValid(PlayerController) == false) 
		return;

	UUserWidget* SpawnedWidget = Cast<UUserWidget>(CreateWidget(PlayerController, WidgetClassToSpawn));
	if (IsValid(SpawnedWidget) == false)
		return;

	SpawnedWidget->AddToViewport();
}

bool USaveLoadPlayerComponent::CanPlayerSaveGame()
{
	return (Player->GetIsInAir() || Player->GetIsInSlowMotion() || 
		Player->GetWeaponHandlerComponent()->GetIsPlayerInAds() || 
		GameInstance->IsPlayerInCombat() || Player->bIsPlayerInElevator || 
		Player->GetIsCrouching()) ? false : true;
}