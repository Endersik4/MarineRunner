// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/SaveLoadPlayerComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Widgets/Menu/GameSavedNotificationWidget.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/Objects/SavedDataObject.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"

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
	GameInstance->bNewGame = false;
}

void USaveLoadPlayerComponent::SaveGame(AActor* JustSavedCheckpoint)
{
	if (IsValid(Player) == false)
		return;

	CreatedSaveGame = Cast<USaveMarineRunner>(UGameplayStatics::CreateSaveGameObject(USaveMarineRunner::StaticClass()));

	CreatedSaveGame->PrepareSaveGame();
	CreatedSaveGame->CopySaveInfoToCurrentGameInstance(GetWorld());
	
	CreatedSaveGame->CurrentHealthSaved = Player->GetHealth();
	CreatedSaveGame->SavedPlayerLocation = Player->GetActorLocation();
	CreatedSaveGame->SavedPlayerRotation = Player->GetActorRotation();

	CreatedSaveGame->Inventory_ItemsSaved = Player->GetInventoryComponent()->Inventory_Items;
	CreatedSaveGame->Inventory_RecipesSaved = Player->GetInventoryComponent()->Items_Recipes;

	Player->GetWeaponInventoryComponent()->SaveInitialWeaponInventory();
	CreatedSaveGame->WeaponInventory_Saved = Player->GetWeaponInventoryComponent()->InitialWeaponInventory;

	SavedDataObject->UpdateObjectsData();
	CreatedSaveGame->SavedCustomData = SavedDataObject->GetCustomSavedData();

	CreatedSaveGame->MakeJsonFileWithSaveInfo(PlayerController, UGameplayStatics::GetCurrentLevelName(GetWorld()));

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

void USaveLoadPlayerComponent::LoadSavedSettingsFromGameInstance()
{
	if (IsValid(GameInstance) == false || IsValid(PlayerController) == false)
		return;

	const FSettingSavedInJsonFile& CurrentMouseSensName = PlayerController->GetMouseSensitivity();
	FSettingSavedInJsonFile MouseSensitivityJson = Player->GetMouseSensitivityJSON();

	GameInstance->FindSavedValueAccordingToName(MouseSensitivityJson.FieldName, MouseSensitivityJson.FieldValue);
	if (CurrentMouseSensName == MouseSensitivityJson) Player->ChangeMouseSensitivity(MouseSensitivityJson);
}

void USaveLoadPlayerComponent::SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn)
{
	if (IsValid(PlayerController) == false) 
		return;

	UGameSavedNotificationWidget* SpawnedWidget = Cast<UGameSavedNotificationWidget>(CreateWidget(PlayerController, WidgetClassToSpawn));
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