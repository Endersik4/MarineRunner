// Copyright Adam Bartela.All Rights Reserved

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
	UFUNCTION(BlueprintImplementableEvent)
		void LoadSoundsVolumeFromConfig(const TArray<FSettingSavedInJsonFile>& AllSavedValuesFromConfig);

	// Get all files with wildcard from SaveGames dir, for example: "ManualSave" will give number of all files with word "ManualSave" in file name
	UFUNCTION(BlueprintImplementableEvent)
		int32 GetAllFilesWithName(const FString& WildCard);

	UFUNCTION(BlueprintCallable)
		void SaveCustomSavedSettingsToConfig();
	UFUNCTION(BlueprintCallable)
		void FindSavedValueAccordingToName(const FString& SavedSettingName, float& Value);
	UFUNCTION(BlueprintCallable)
		void ReplaceValueInSavedSettingByName(float NewValue, const FString& SavedSettingName);

	void SetSaveNumberAccordingToNumOfFiles(const FString& WildCard = "*ManualSave*");

	void AddNewDetectedEnemy(TObjectPtr<AActor> NewEnemy);
	void RemoveDetectedEnemy(TObjectPtr<AActor> NewEnemy);
	void ResetDetectedEnemy();
	FORCEINLINE bool IsPlayerInCombat() const { return DetectedPlayerEnemies.Num() > 0; }

	void ChangeBackgroundMusic(EMusicType MusicType, bool bIgnoreFadeOut = false);
	FORCEINLINE EMusicType GetCurrentMusicType() const { return CurrentMusicType; }
	FORCEINLINE void SetCurrentExplorationMusic(TObjectPtr<USoundBase> MusicToSet) { CurrentExplorationMusic = MusicToSet; }
	FORCEINLINE TObjectPtr<USoundBase> GetCurrentExplorationMusic() const { return CurrentExplorationMusic; }

	bool bNewGame = false;

	UPROPERTY(EditAnywhere, Category = "Saving Game")
		FString CustomSavedSettingsConfigPath = "MarineRunner/Config/Settings.json";
	UPROPERTY(EditAnywhere, Category = "Saving Game")
		FString SlotSaveGameNameToLoad = "MySlot";
	UPROPERTY(EditAnywhere, Category = "Saving Game")
		int32 CurrentSaveNumber = 0;
	UPROPERTY(EditAnywhere, Category = "Saving Game")
		float TotalPlayTimeInSeconds = 0.f;
	UPROPERTY(EditAnywhere, Category = "Saving Game")
		float LastGameTimePlayTime = 0.f;
private:
	UPROPERTY(EditAnywhere, Category = "Saving")
		TArray<FSettingSavedInJsonFile> CustomSavedSettings;

	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
		float NewMusicFadeIn = 2.f;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
		float OldMusicFadeOut = 2.f;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
		TObjectPtr<USoundBase> MainMenuMusic;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
		TObjectPtr<USoundBase> CombatMusic;

	UPROPERTY(Transient)
		TObjectPtr<USoundBase> CurrentExplorationMusic;
	UPROPERTY(Transient)
		TObjectPtr<UAudioComponent> CurrentPlayingMusic;


	bool bWasJsonDeserialized;
	TSharedPtr<FJsonObject> SavedDataJsonFile;
	void LoadCustomSavedSettingsFromConfig();

	// Enemies that see the player
	UPROPERTY(Transient)
		TArray<TObjectPtr<AActor>> DetectedPlayerEnemies;
	bool bIsDetectedByEnemies = false;

	// Background music
	FTimerHandle BackgroundMusicHandle;
	EMusicType CurrentMusicType;
	void SpawnBackgroundMusic(TObjectPtr<USoundBase> SoundToSpawn);
	void ChangeMusicAfterFadeOut();
};