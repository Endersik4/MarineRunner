// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"
#include "MarineRunner/Widgets/HUDWidget.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "MarineRunner/GunClasses/Gun.h"

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
	if (!GameInstance->bNewGame)
		return;
	
	SpawnPassingWidget(NewGameBeginsWidgetClass);
	GameInstance->bNewGame = false;

	if (!IsValid(Player))
		return;

	if (IsValid(Player->GetHudWidget()))
		Player->GetHudWidget()->NewGameStartedWidgetAnimation();
}

void USaveLoadPlayerComponent::SaveGame(const FString& _SaveName, const FString& _WildCard)
{
	if (!IsValid(Player))
		return;

	TObjectPtr<USaveMarineRunner> CreatedSaveGame = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));
	if (!IsValid(CreatedSaveGame))
		return;

	CreatedSaveGame->PrepareSaveGame(_SaveName);
	CreatedSaveGame->TransferSavedVariablesToGameInstance(GetWorld(), _WildCard);
	
	CreatedSaveGame->CurrentHealthSaved = Player->GetHealth();
	CreatedSaveGame->SavedPlayerLocation = Player->GetActorLocation();
	CreatedSaveGame->SavedPlayerRotation = Player->GetActorRotation();

	CreatedSaveGame->bShowHealBar = bShowHealBar;
	CreatedSaveGame->bShowDashBar = bShowDashBar;
	CreatedSaveGame->bShowSlowMotionBar = bShowSlowMotionBar;

	CreatedSaveGame->Inventory_ItemsSaved = Player->GetInventoryComponent()->Inventory_Items;
	CreatedSaveGame->Inventory_RecipesSaved = Player->GetInventoryComponent()->Items_Recipes;

	Player->GetWeaponInventoryComponent()->WeaponStorageToInitialWeaponInventory();
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
	if (!IsValid(GameInstance) || !IsValid(Player))
		return;

	if (GameInstance->bNewGame)
		return;
	
	TObjectPtr<USaveMarineRunner> LoadGameInstance = CreateLoadGame();
	if (!IsValid(LoadGameInstance))
		return;
	
	GameInstance->ResetDetectedEnemy();
	GameInstance->ChangeBackgroundMusic(EMT_Exploration, true);

	bShowHealBar = LoadGameInstance->bShowHealBar;
	bShowDashBar = LoadGameInstance->bShowDashBar;
	bShowSlowMotionBar = LoadGameInstance->bShowSlowMotionBar;

	Player->SetActorLocation(LoadGameInstance->SavedPlayerLocation);
	Player->SetActorRotation(LoadGameInstance->SavedPlayerRotation);
	if (IsValid(PlayerController))
		PlayerController->SetControlRotation(LoadGameInstance->SavedPlayerRotation);

	LoadGameInstance->LoadGame(Player, GameInstance);
	LoadGameInstance->LoadOtherObjectsData(SavedDataObject);
}

void USaveLoadPlayerComponent::LoadHudVariables()
{
	if (!IsValid(Player->GetHudWidget()))
		return;

	Player->GetHudWidget()->ShowGameplayMechanicsBars(bShowHealBar, bShowDashBar, bShowSlowMotionBar);
}

void USaveLoadPlayerComponent::ShowGameplayMechanicsOnHud(const EUnlockInHud& WhatToUnlock)
{
	if (WhatToUnlock == EUnlockInHud::EUIN_DashBar)
		bShowDashBar = true;
	else if (WhatToUnlock == EUnlockInHud::EUIN_HealBar)
		bShowHealBar = true;
	else if (WhatToUnlock == EUnlockInHud::EUIN_SlowMoBar)
		bShowSlowMotionBar = true;

	if (!IsValid(Player->GetHudWidget()))
		return;

	Player->GetHudWidget()->ShowGameplayMechanicsBars(WhatToUnlock);
}

void USaveLoadPlayerComponent::RestartGame()
{
	if (!IsValid(Player))
		return;

	auto DeleteAllActorsOfClass = [](UWorld* World,UClass* ClassToDelete)
	{
		TArray<TObjectPtr<AActor>> ActorsToDelete;
		UGameplayStatics::GetAllActorsOfClass(World, ClassToDelete, ActorsToDelete);

		for (TObjectPtr<AActor> CurrentActor : ActorsToDelete)
		{
			if (!IsValid(CurrentActor))
				continue;
			CurrentActor->Destroy();
		}
		ActorsToDelete.Empty();
	};

	DeleteAllActorsOfClass(GetWorld(), AGun::StaticClass());
	DeleteAllActorsOfClass(GetWorld(), AEnemyPawn::StaticClass());

	SavedDataObject->RestartObjectsData(); // Restart Objects on map

	const float& DefaultTimeDilation = 1.f;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), DefaultTimeDilation);
	UGameplayStatics::SetGlobalPitchModulation(GetWorld(), DefaultTimeDilation, UGameplayStatics::GetWorldDeltaSeconds(GetWorld()));

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	SpawnNewPlayer();

	GameInstance->ResetDetectedEnemy();
	UGameplayStatics::GetPlayerCameraManager(GetWorld(),0)->StopAllCameraShakes();

	Player->Destroy();
}

void USaveLoadPlayerComponent::SpawnNewPlayer()
{
	FTransform NewPlayerTransform = FTransform(FRotator(0.f), FVector(0.f));

	TObjectPtr<USaveMarineRunner> LoadGameInstance = CreateLoadGame();
	if (IsValid(LoadGameInstance))
		NewPlayerTransform = FTransform(LoadGameInstance->SavedPlayerRotation, LoadGameInstance->SavedPlayerLocation);

	TObjectPtr<AMarineCharacter> SpawnedNewPlayer = GetWorld()->SpawnActorDeferred<AMarineCharacter>(PlayerClass, NewPlayerTransform);
	if (!IsValid(SpawnedNewPlayer))
		return;

	SpawnedNewPlayer->SetAlbertosPawn(Player->GetAlbertosPawn());
	SpawnedNewPlayer->GetSaveLoadPlayerComponent()->SetSavedDataObject(Player->GetSaveLoadPlayerComponent()->GetSavedDataObject());
	Player->GetAlbertosPawn()->GetPlayerIsNearComponent()->SetPlayerPawn(SpawnedNewPlayer);

	SpawnedNewPlayer->FinishSpawning(NewPlayerTransform);

	PlayerController->Possess(SpawnedNewPlayer);
	SpawnedNewPlayer->ReplaceRootComponentRotation();
	PlayerController->SetMouseSensitivity(Player->GetMouseSensitivityJSON());

	if (IsValid(LoadGameInstance)&& IsValid(GameInstance))
		GameInstance->LastGameTimePlayTime = LoadGameInstance->LastGameTimePlayTime;
}

TObjectPtr<USaveMarineRunner> USaveLoadPlayerComponent::CreateLoadGame()
{
	FString SlotName = GameInstance->SlotSaveGameNameToLoad;
	SlotName += "/" + SlotName;

	return Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
}

void USaveLoadPlayerComponent::LoadSavedSettingsFromGameInstance()
{
	if (!IsValid(GameInstance) || !IsValid(PlayerController))
		return;

	Player->LoadFieldOfViewFromSettings();

	const FSettingSavedInJsonFile& CurrentMouseSensName = PlayerController->GetMouseSensitivity();
	FSettingSavedInJsonFile MouseSensitivityJson = Player->GetMouseSensitivityJSON();

	GameInstance->FindSavedValueAccordingToName(MouseSensitivityJson.FieldName, MouseSensitivityJson.FieldValue);
	if (CurrentMouseSensName == MouseSensitivityJson) 
		Player->ChangeMouseSensitivity(MouseSensitivityJson);
}

void USaveLoadPlayerComponent::SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn)
{
	if (!IsValid(PlayerController)) 
		return;

	TObjectPtr<UUserWidget> SpawnedWidget = Cast<UUserWidget>(CreateWidget(PlayerController, WidgetClassToSpawn));
	if (!IsValid(SpawnedWidget))
		return;

	SpawnedWidget->AddToViewport();
}

bool USaveLoadPlayerComponent::CanPlayerSaveGame()
{
	if (!IsValid(Player) || !IsValid(GameInstance))
		return false;

	if (Player->GetIsInAir() || Player->GetIsInSlowMotion())
		return false;

	if (GameInstance->IsPlayerInCombat() || Player->bIsPlayerInElevator)
		return false;

	if (Player->GetIsCrouching())
		return false;

	return true;
}