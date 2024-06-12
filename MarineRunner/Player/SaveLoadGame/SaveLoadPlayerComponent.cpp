// Copyright Adam Bartela.All Rights Reserved

#include "SaveLoadPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "SaveMarineRunner.h"
#include "Objects/SavedDataObject.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/Components/PlayerIsNearAlbertosComponent.h"
#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"
#include "MarineRunner/Weapon/Gun/Gun.h"

// Sets default values for this component's properties
USaveLoadPlayerComponent::USaveLoadPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USaveLoadPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Save Load Player Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}
	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player Controller is nullptr in Save Load Player Component!")))
	{
		PlayerController = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	if (ensureMsgf(IsValid(UGameplayStatics::GetGameInstance(GetWorld())), TEXT("Game Instance is nullptr in Save Load Player Component!")))
	{
		GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	}
	
	LoadSavedSettingsFromGameInstance();

	LoadGame();
}

void USaveLoadPlayerComponent::SpawnNewGameWidget()
{
	if (!IsValid(GameInstance))
		return;

	SpawnPassingWidget(GameLoadedWidgetClass);

	if (!GameInstance->bNewGame)
		return;
	
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

	CreatedSaveGame->CheckpointNumber = Player->CheckpointNumber; //ztgk, delete later

	CreatedSaveGame->bShowHealBar = bShowHealBar;
	CreatedSaveGame->bShowDashBar = bShowDashBar;
	CreatedSaveGame->bShowSlowMotionBar = bShowSlowMotionBar;

	CreatedSaveGame->Inventory_ItemsSaved = Player->GetInventoryComponent()->Inventory_Items;
	CreatedSaveGame->Inventory_RecipesSaved = Player->GetInventoryComponent()->Items_Recipes;

	Player->GetWeaponInventoryComponent()->WeaponStorageToInitialWeaponInventory();
	CreatedSaveGame->WeaponInventory_Saved = Player->GetWeaponInventoryComponent()->WeaponsToLoadToInventory;

	SavedDataObject->RemoveTempCustomSavedData();
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

bool USaveLoadPlayerComponent::GetIsGameplayMechanicEnabled(const EUnlockInHud& WhatToGet)
{
	if (WhatToGet == EUnlockInHud::EUIN_DashBar)
		return bShowDashBar;
	else if (WhatToGet == EUnlockInHud::EUIN_HealBar)
		return bShowHealBar;
	else if (WhatToGet == EUnlockInHud::EUIN_SlowMoBar)
		return bShowSlowMotionBar;

	return false;
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
	if (IsValid(Player->GetAlbertosPawn()))
		Player->GetAlbertosPawn()->GetPlayerIsNearComponent()->SetPlayerPawn(SpawnedNewPlayer);

	SpawnedNewPlayer->FinishSpawning(NewPlayerTransform);

	PlayerController->Possess(SpawnedNewPlayer);
	SpawnedNewPlayer->ReplaceRootComponentRotation();
	PlayerController->SetMouseSensitivity(Player->GetMouseSensitivityJSON());

	if (IsValid(LoadGameInstance) && IsValid(GameInstance))
		GameInstance->LastGameTimePlayTime = LoadGameInstance->LastGameTimePlayTime;
}

TObjectPtr<USaveMarineRunner> USaveLoadPlayerComponent::CreateLoadGame()
{
	const FString& SlotName = GameInstance->SlotSaveGameNameToLoad + "/" + GameInstance->SlotSaveGameNameToLoad;
	return Cast<USaveMarineRunner>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
}

void USaveLoadPlayerComponent::LoadSavedSettingsFromGameInstance()
{
	if (!IsValid(GameInstance) || !IsValid(PlayerController) || !IsValid(Player))
		return;

	Player->LoadFieldOfViewFromSettings();

	const FSettingSavedInJsonFile& CurrentMouseSensName = PlayerController->GetMouseSensitivity();
	FSettingSavedInJsonFile MouseSensitivityJson = Player->GetMouseSensitivityJSON();

	MouseSensitivityJson.FieldValue = GameInstance->FindSavedValueAccordingToName(MouseSensitivityJson.FieldName);
	if (CurrentMouseSensName == MouseSensitivityJson) 
		Player->ChangeMouseSensitivity(MouseSensitivityJson);
}

void USaveLoadPlayerComponent::SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn)
{
	if (!IsValid(PlayerController) || !IsValid(WidgetClassToSpawn)) 
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