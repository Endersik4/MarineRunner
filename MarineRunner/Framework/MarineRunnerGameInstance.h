// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MarineRunnerGameInstance.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSettingSavedInJsonFile {
	GENERATED_USTRUCT_BODY();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString FieldName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float FieldValue;

	FSettingSavedInJsonFile() {
		FieldName = "";
		FieldValue = 0.f;
	}

	FSettingSavedInJsonFile(FString NewFieldName, float NewFieldValue)
	{
		FieldName = NewFieldName;
		FieldValue = NewFieldValue;
	}

	FORCEINLINE bool operator==(const FSettingSavedInJsonFile& SavedSetting) const
	{
		return FieldName == SavedSetting.FieldName;
	}

	int32 GetItemIndexInGivenArray(const TArray<FSettingSavedInJsonFile>& Array)
	{
		int32 Index = 0;
		for (const FSettingSavedInJsonFile& CurrentStruct : Array)
		{
			if (*this == CurrentStruct)
			{
				return Index;
			}
			Index++;
		}
		return INDEX_NONE;
	}
};

enum EMusicType {
	EMT_Exploration,
	EMT_PauseMusic,
	EMT_Combat
};

UCLASS()
class MARINERUNNER_API UMarineRunnerGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:
		virtual void Init() override;
public:
	UPROPERTY(EditAnywhere, Category = "Saving")
		FString CustomSavedSettingsConfigPath = "MarineRunner/Config/Settings.json";
	UPROPERTY(EditAnywhere, Category = "Saving")
		FString SlotSaveGameNameToLoad = "MySlot";
	UPROPERTY(EditAnywhere, Category = "Saving")
		int32 CurrentSaveNumber = 0;
	UPROPERTY(EditAnywhere, Category = "Saving")
		float TotalPlayTimeInSeconds = 0.f;

	UPROPERTY(EditAnywhere, Category = "Saving")
		TArray<FSettingSavedInJsonFile> CustomSavedSettings;

	UPROPERTY(EditAnywhere, Category = "Music")
		USoundBase* MainMenuMusic;
	UPROPERTY(EditAnywhere, Category = "Music")
		USoundBase* ExplorationMusic;
	UPROPERTY(EditAnywhere, Category = "Music")
		USoundBase* CombatMusic;

	UFUNCTION(BlueprintImplementableEvent)
		void LoadSoundsVolumeFromConfig(const TArray<FSettingSavedInJsonFile> & AllSavedValuesFromConfig);

	// Get all files with wildcard from SaveGames dir, for example: "ManualSave" will give number of all files with word "ManualSave" in file name
	UFUNCTION(BlueprintImplementableEvent)
		int32 GetAllFilesWithName(const FString& WildCard);

	void SetSaveNumberAccordingToNumOfFiles();

	void SaveCustomSavedSettingsToConfig();

	void FindSavedValueAccordingToName(const FString& SavedSettingName, float& Value);
	void ReplaceValueInSavedSettingByName(float NewValue, const FString& SavedSettingName);

	void AddNewDetectedEnemy(AActor* NewEnemy, bool bIsEnemyDead = false);
	void RemoveDetectedEnemy(AActor* NewEnemy);
	FORCEINLINE bool IsPlayerInCombat() const { return DetectedPlayerEnemies.Num() > 0; }

	void ChangeBackgroundMusic(EMusicType MusicType, bool bIgnoreFadeOut = false);

	FORCEINLINE EMusicType GetCurrentMusicType() const {return CurrentMusicType;}

	bool bNewGame = false;
private:

	bool bWasJsonDeserialized;
	TSharedPtr<FJsonObject> SavedDataJsonFile;
	void LoadCustomSavedSettingsFromConfig();

	// Enemies that see the player
	TArray<AActor*> DetectedPlayerEnemies;
	bool bIsDetectedByEnemies = false;

	// Background music
	FTimerHandle BackgroundMusicHandle;
	EMusicType CurrentMusicType;
	//void SpawnBackgroundMusicFirstTime();
	UFUNCTION()
		void SpawnBackgroundMusic(USoundBase* SoundToSpawn);
	UPROPERTY()
		UAudioComponent* CurrentPlayingMusic;
	void ChangeMusicAfterFadeOut();

};
