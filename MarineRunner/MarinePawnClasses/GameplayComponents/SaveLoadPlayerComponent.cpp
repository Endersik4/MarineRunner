// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

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

	USaveMarineRunner* CreatedSaveGame = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));

	CreatedSaveGame->PrepareSaveGame(_SaveName);
	CreatedSaveGame->CopySaveInfoToCurrentGameInstance(GetWorld(), _WildCard);
	
	CreatedSaveGame->CurrentHealthSaved = Player->GetHealth();
	CreatedSaveGame->SavedPlayerLocation = Player->GetActorLocation();
	CreatedSaveGame->SavedPlayerRotation = Player->GetActorRotation();

	CreatedSaveGame->Inventory_ItemsSaved = Player->GetInventoryComponent()->Inventory_Items;
	CreatedSaveGame->Inventory_RecipesSaved = Player->GetInventoryComponent()->Items_Recipes;

	Player->GetWeaponInventoryComponent()->SaveInitialWeaponInventory();
	CreatedSaveGame->WeaponInventory_Saved = Player->GetWeaponInventoryComponent()->InitialWeaponInventory;

	SavedDataObject->RestartObjectsData(true);
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
	
	USaveMarineRunner* LoadGameInstance = CreateLoadGame();
	if (IsValid(LoadGameInstance) == false)
		return;
	
	GameInstance->ResetDetectedEnemy();
	GameInstance->ChangeBackgroundMusic(EMT_Exploration, true);
	
	Player->SetActorLocation(LoadGameInstance->SavedPlayerLocation);
	Player->SetActorRotation(LoadGameInstance->SavedPlayerRotation);
	if (IsValid(PlayerController) == true)
		PlayerController->SetControlRotation(LoadGameInstance->SavedPlayerRotation);
	
	LoadGameInstance->LoadGame(Player, GameInstance);
	LoadGameInstance->LoadOtherObjectsData(SavedDataObject);
}

void USaveLoadPlayerComponent::RestartGame()
{
	if (IsValid(Player) == false)
		return;

	TArray<AActor*> AllGunsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGun::StaticClass(), AllGunsOnMap);
	for (AActor* CurrentEnemy : AllGunsOnMap)
	{
		CurrentEnemy->Destroy();
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass(), AllGunsOnMap);
	for (AActor* CurrentEnemy : AllGunsOnMap)
	{
		CurrentEnemy->Destroy();
	}

	SavedDataObject->RestartObjectsData(); // Restart Objects on map

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), 1.f, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	SpawnNewPlayer();

	GameInstance->ResetDetectedEnemy();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->StopAllCameraShakes();

	Player->Destroy();
}

void USaveLoadPlayerComponent::SpawnNewPlayer()
{
	FTransform NewPlayerTransform = FTransform(FRotator(0.f), FVector(0.f));

	USaveMarineRunner* LoadGameInstance = CreateLoadGame();
	if (IsValid(LoadGameInstance) == true)
		NewPlayerTransform = FTransform(LoadGameInstance->SavedPlayerRotation, LoadGameInstance->SavedPlayerLocation);

	AMarineCharacter* SpawnedNewPlayer = GetWorld()->SpawnActorDeferred<AMarineCharacter>(PlayerClass, NewPlayerTransform);
	if (IsValid(SpawnedNewPlayer) == false)
		return;

	SpawnedNewPlayer->SetAlbertosPawn(Player->GetAlbertosPawn());
	SpawnedNewPlayer->GetSaveLoadPlayerComponent()->SetSavedDataObject(Player->GetSaveLoadPlayerComponent()->GetSavedDataObject());
	Player->GetAlbertosPawn()->GetPlayerIsNearComponent()->SetPlayerPawn(SpawnedNewPlayer);

	SpawnedNewPlayer->FinishSpawning(NewPlayerTransform);

	PlayerController->Possess(SpawnedNewPlayer);
	SpawnedNewPlayer->ReplaceRootComponentRotation();
	PlayerController->SetMouseSensitivity(Player->GetMouseSensitivityJSON());

	if (IsValid(LoadGameInstance) == true && IsValid(GameInstance) == true)
		GameInstance->LastGameTimePlayTime = LoadGameInstance->LastGameTimePlayTime;
}

USaveMarineRunner* USaveLoadPlayerComponent::CreateLoadGame()
{
	FString SlotName = GameInstance->SlotSaveGameNameToLoad;
	SlotName += "/" + SlotName;

	USaveMarineRunner* LoadGameInstance = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	if (UGameplayStatics::LoadGameFromSlot(SlotName, 0) == nullptr)
	{
		return nullptr;
	}

	return Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
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