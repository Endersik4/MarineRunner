// Copyright Adam Bartela.All Rights Reserved

#include "SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/SaveLoadGame/JsonFileActions.h"
#include "MarineRunner/Player/SaveLoadGame/Objects/SavedDataObject.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

USaveMarineRunner::USaveMarineRunner()
{
	
}
void USaveMarineRunner::PrepareSaveGame(const FString& NewSaveName, bool bNewAddSaveNumber)
{
	OriginalSaveName = NewSaveName;
	bAddSaveNumber = bNewAddSaveNumber;
}

FString USaveMarineRunner::GetSaveGameName()
{
	return OriginalSaveName + (bAddSaveNumber ? FString::FromInt(SaveNumber) : "");
}

#pragma region ///////// SAVING ////////////
void USaveMarineRunner::TransferSavedVariablesToGameInstance(TObjectPtr<UWorld> CurrentWorld, const FString& _WildCard)
{
	if (!IsValid(UGameplayStatics::GetGameInstance(CurrentWorld)))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(CurrentWorld));
	if (!IsValid(GameInstance))
		return;

	GameInstance->SetSaveNumberAccordingToNumOfFiles(_WildCard);
	GameInstance->CurrentSaveNumber++;
	SaveNumber = GameInstance->CurrentSaveNumber;

	GameInstance->SlotSaveGameNameToLoad = GetSaveGameName();

	GameInstance->TotalPlayTimeInSeconds += (UKismetSystemLibrary::GetGameTimeInSeconds(CurrentWorld) - GameInstance->LastGameTimePlayTime);
	GameInstance->LastGameTimePlayTime = UKismetSystemLibrary::GetGameTimeInSeconds(CurrentWorld);

	TotalPlayTimeInSeconds = GameInstance->TotalPlayTimeInSeconds;
	LastGameTimePlayTime = GameInstance->LastGameTimePlayTime;
}

FString USaveMarineRunner::TakeSaveScreenshot(TObjectPtr<APlayerController> PlayerController)
{
	if (!IsValid(PlayerController))
		return "0";

	const FString& ScreenshotName = GetSaveGameName() +".jpg";
	const FString& SaveGamesDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + ScreenshotName;
	const FString& TakeScreenShotCommand = "HighResShot 320x180 filename=\"" + SaveGamesDir + "\"";

	PlayerController->ConsoleCommand(TakeScreenShotCommand);
	return SaveGamesDir;
}

FString USaveMarineRunner::ConvertCurrentDateToText()
{
	const FString& CurrentTimeText_Minutes = FDateTime::Now().GetMinute() < 10 ? "0" + FString::FromInt(FDateTime::Now().GetMinute()) : FString::FromInt(FDateTime::Now().GetMinute());
	const FString& CurrentTimeText = FString::FromInt(FDateTime::Now().GetHour()) + ":" + CurrentTimeText_Minutes;

	const FString& SaveDateText = CurrentTimeText + " " + FString::FromInt(FDateTime::Now().GetDay()) + "/" + FString::FromInt(FDateTime::Now().GetMonth()) + "/" + FString::FromInt(FDateTime::Now().GetYear());
	return SaveDateText;
}

void USaveMarineRunner::MakeJsonFileWithSaveInfo(TObjectPtr<APlayerController> PlayerController, const FString& CurrentLevelName)
{
	const FString& PathToScreenshot = TakeSaveScreenshot(PlayerController);
	const FString& SaveDateText = ConvertCurrentDateToText();

	const FString& FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + GetSaveGameName() + ".json";

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("SavedDataName", GetSaveGameName());
	JsonObject->SetNumberField("SavedDataNumber", SaveNumber);
	JsonObject->SetStringField("PathToThumbnail", PathToScreenshot);
	JsonObject->SetStringField("SavedGameDate", ConvertCurrentDateToText());
	JsonObject->SetNumberField("SavedDateValue", FMath::RoundToInt32(FDateTime::Now().ToUnixTimestampDecimal() / 10.f));
	JsonObject->SetStringField("SavedLevelName", CurrentLevelName);
	JsonObject->SetNumberField("TotalPlayTime", TotalPlayTimeInSeconds);
	USaveGameJsonFile::WriteJson(JsonObject, FilePath);
}
#pragma endregion

#pragma region ////////// LOADING //////////////
void USaveMarineRunner::LoadGame(TObjectPtr<AMarineCharacter> MarinePawn, TObjectPtr<UMarineRunnerGameInstance> GameInstance)
{
	if (!IsValid(MarinePawn)) 
		return;

	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->GetInventoryComponent()->Inventory_Items = Inventory_ItemsSaved;
	MarinePawn->GetInventoryComponent()->Items_Recipes = Inventory_RecipesSaved;

	MarinePawn->GetWeaponInventoryComponent()->InitialWeaponInventory = WeaponInventory_Saved;
	MarinePawn->GetWeaponInventoryComponent()->StartTimerForSpawnNewWeapons();

	if (!IsValid(GameInstance)) 
		return;

	GameInstance->SetSaveNumberAccordingToNumOfFiles();
	GameInstance->TotalPlayTimeInSeconds = TotalPlayTimeInSeconds;
	GameInstance->LastGameTimePlayTime = 0.f;

	if (GameInstance->GetCurrentExplorationMusic() != SavedExplorationMusic)
	{
		GameInstance->SetCurrentExplorationMusic(SavedExplorationMusic);
		GameInstance->ChangeBackgroundMusic(EMusicType::EMT_Exploration, true);
	}
}

void USaveMarineRunner::LoadOtherObjectsData(TObjectPtr<ASavedDataObject> OtherObjectsData)
{
	if (!IsValid(OtherObjectsData))
		return;

	OtherObjectsData->SetCustomSavedData(SavedCustomData);
	OtherObjectsData->RemoveTempCustomSavedData();
	OtherObjectsData->StartLoadingObjects();
}

#pragma endregion