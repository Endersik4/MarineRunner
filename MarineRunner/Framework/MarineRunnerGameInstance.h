// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MarineRunnerGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FGameDifficulty
{
public:
	GENERATED_USTRUCT_BODY();

	// numerical representation of difficulties e.g 0 - easy, 1 - normal, 2 - hard
	UPROPERTY(EditAnywhere)
	int32 DifficultyLevel = 0;
	UPROPERTY(EditAnywhere)
	FText DifficultyName = FText();
	UPROPERTY(EditAnywhere, meta = (MultiLine = "true"))
	FText DifficultyDescription = FText();
	// Multiplies EnemiesDifficultyPercent by enemies' stats, e.g. damage *= 0.5f
	UPROPERTY(EditAnywhere)
	float EnemiesDifficultyPercent = 1.f;

	FGameDifficulty()
	{
		DifficultyLevel = 0;
		DifficultyName = FText();
		DifficultyDescription = FText();
		EnemiesDifficultyPercent = 1.f;
	}

	FGameDifficulty(const int32& _DifficultyLevel, const FText& _DifficultyName, const FText& _DifficultyDesc, const float& _EnemiesDifficultyPercent)
	{
		DifficultyLevel = _DifficultyLevel;
		DifficultyName = _DifficultyName;
		DifficultyDescription = _DifficultyDesc;
		EnemiesDifficultyPercent = _EnemiesDifficultyPercent;
	}

	FORCEINLINE bool operator==(const FGameDifficulty& OtherGameDifficulty) const
	{
		return DifficultyLevel == OtherGameDifficulty.DifficultyLevel;
	}

	FORCEINLINE bool operator==(const int32& OtherGameDifficultyLevel) const
	{
		return DifficultyLevel == OtherGameDifficultyLevel;
	}
};

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

UENUM()
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
	const int32 GetAllFilesWithName(const FString& WildCard);

	UFUNCTION(BlueprintCallable)
	void SaveCustomSavedSettingsToConfig();
	UFUNCTION(BlueprintCallable)
	float FindSavedValueAccordingToName(const FString& SavedSettingName);
	UFUNCTION(BlueprintCallable)
	void ReplaceValueInSavedSettingByName(float NewValue, const FString& SavedSettingName);

	void SetSaveNumberAccordingToNumOfFiles(const FString& WildCard = "*ManualSave*");

	void AddNewDetectedEnemy(const TObjectPtr<AActor> NewEnemy);
	void RemoveDetectedEnemy(const TObjectPtr<AActor> NewEnemy);
	void ResetDetectedEnemy();
	FORCEINLINE const bool IsPlayerInCombat() const { return DetectedPlayerEnemies.Num() > 0; }

	void ChangeBackgroundMusic(EMusicType MusicType, bool bIgnoreFadeOut = false);
	FORCEINLINE void SetCurrentExplorationMusic(TObjectPtr<USoundBase> MusicToSet) { CurrentExplorationMusic = MusicToSet; }
	FORCEINLINE EMusicType GetCurrentMusicType() const { return CurrentMusicType; }
	FORCEINLINE TObjectPtr<USoundBase> GetCurrentExplorationMusic() const { return CurrentExplorationMusic; }

	FORCEINLINE const TArray<FGameDifficulty>& GetAllGameDifficulties() const { return AllGameDifficulties; };
	FORCEINLINE const FString& GetGameDifficultySavedFieldName() const { return GameDifficultySavedFieldName; };

	UPROPERTY(Transient)
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

	UPROPERTY(EditAnywhere, Category = "Game Difficulty")
	FString GameDifficultySavedFieldName = TEXT("GameDifficulty");
	UPROPERTY(EditAnywhere, Category = "Game Difficulty", meta = (Multiline = "true"))
	TArray<FGameDifficulty> AllGameDifficulties;

	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
	float NewMusicFadeIn = 2.f;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
	float OldMusicFadeOut = 2.f;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
	TObjectPtr<USoundBase> MainMenuMusic = nullptr;
	UPROPERTY(EditAnywhere, Category = "Dynamic Music")
	TObjectPtr<USoundBase> CombatMusic = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<USoundBase> CurrentExplorationMusic = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> CurrentPlayingMusic = nullptr;

	UPROPERTY(Transient)
	bool bWasJsonDeserialized = false;
	TSharedPtr<FJsonObject> SavedDataJsonFile = nullptr;
	void LoadCustomSavedSettingsFromConfig();

	// Enemies that see the player
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> DetectedPlayerEnemies;
	UPROPERTY(Transient)
	bool bIsDetectedByEnemies = false;

	UPROPERTY(Transient)
	TEnumAsByte<EMusicType> CurrentMusicType = EMusicType::EMT_Exploration;
	FTimerHandle BackgroundMusicHandle;
	void SpawnBackgroundMusic(TObjectPtr<USoundBase> SoundToSpawn);
	void ChangeMusicAfterFadeOut();
};