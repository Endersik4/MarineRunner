// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Framework/SaveMarineRunner.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"
#include "MarineRunner/SaveGame/SaveGameJsonFile.h"
#include "MarineRunner/Objects/SavedDataObject.h"

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
void USaveMarineRunner::CopySaveInfoToCurrentGameInstance(UWorld* CurrentWorld, const FString& _WildCard)
{
	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(CurrentWorld));
	if (IsValid(GameInstance) == false)
		return;

	GameInstance->SetSaveNumberAccordingToNumOfFiles(_WildCard);
	GameInstance->CurrentSaveNumber++;
	SaveNumber = GameInstance->CurrentSaveNumber;

	GameInstance->SlotSaveGameNameToLoad = GetSaveGameName();
	GameInstance->TotalPlayTimeInSeconds += UKismetSystemLibrary::GetGameTimeInSeconds(CurrentWorld);

	TotalPlayTimeInSeconds = GameInstance->TotalPlayTimeInSeconds;
}

FString USaveMarineRunner::TakeSaveScreenshot(APlayerController* PlayerController)
{
	if (IsValid(PlayerController) == false) return "0";

	FString ScreenshotName = GetSaveGameName() +".jpg";
	FString SaveGamesDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + ScreenshotName;
	FString TakeScreenShotCommand = "HighResShot 320x180 filename=\"" + SaveGamesDir + "\"";

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

void USaveMarineRunner::MakeJsonFileWithSaveInfo(APlayerController* PlayerController, const FString& CurrentLevelName)
{
	const FString& PathToScreenshot = TakeSaveScreenshot(PlayerController);
	const FString& SaveDateText = ConvertCurrentDateToText();

	FString FilePath = UKismetSystemLibrary::GetProjectSavedDirectory() + "SaveGames/" + GetSaveGameName() + "/" + GetSaveGameName() + ".json";

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("SavedDataName", GetSaveGameName());
	JsonObject->SetNumberField("SavedDataNumber", SaveNumber);
	JsonObject->SetStringField("PathToThumbnail", PathToScreenshot);
	JsonObject->SetStringField("SavedGameDate", ConvertCurrentDateToText());
	JsonObject->SetStringField("SavedLevelName", CurrentLevelName);
	JsonObject->SetNumberField("TotalPlayTime", TotalPlayTimeInSeconds);

	USaveGameJsonFile::WriteJson(JsonObject, FilePath);
}
#pragma endregion

#pragma region ////////// LOADING //////////////
void USaveMarineRunner::LoadGame(AMarineCharacter* MarinePawn, UMarineRunnerGameInstance* GameInstance)
{
	if (IsValid(MarinePawn) == false) 
		return;

	MarinePawn->SetHealth(CurrentHealthSaved);
	MarinePawn->GetInventoryComponent()->Inventory_Items = Inventory_ItemsSaved;
	MarinePawn->GetInventoryComponent()->Items_Recipes = Inventory_RecipesSaved;

	MarinePawn->GetWeaponInventoryComponent()->InitialWeaponInventory = WeaponInventory_Saved;

	if (IsValid(GameInstance) == false) 
		return;

	GameInstance->SetSaveNumberAccordingToNumOfFiles();
	GameInstance->TotalPlayTimeInSeconds = TotalPlayTimeInSeconds;

	if (GameInstance->GetCurrentExplorationMusic() != SavedExplorationMusic)
	{
		GameInstance->SetCurrentExplorationMusic(SavedExplorationMusic);
		GameInstance->ChangeBackgroundMusic(EMusicType::EMT_Exploration, true);
	}
}

void USaveMarineRunner::LoadOtherObjectsData(ASavedDataObject* OtherObjectsData)
{
	if (IsValid(OtherObjectsData) == false)
		return;

	OtherObjectsData->SetCustomSavedData(SavedCustomData);
	OtherObjectsData->EmptyTempCustomSavedData();
	OtherObjectsData->LoadObjectsData();
}

#pragma endregion