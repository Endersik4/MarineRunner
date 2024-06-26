// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveLoadPlayerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API USaveLoadPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USaveLoadPlayerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	void SaveGame(const FString& _SaveName = "ManualSave_", const FString& _WildCard = "*ManualSave*");
	bool CanPlayerSaveGame();
	void SpawnCannotSavedWidget() { SpawnPassingWidget(CannotSavedNotificationWidgetClass); }

	void LoadSavedSettingsFromGameInstance();

	void LoadGame();
	void RestartGame();
	void SpawnNewGameWidget();
	void LoadHudVariables();

	FORCEINLINE void SetSavedDataObject(TObjectPtr<class ASavedDataObject> NewSavedDataObject) { SavedDataObject = NewSavedDataObject; }
	FORCEINLINE TObjectPtr<class ASavedDataObject> GetSavedDataObject() { return SavedDataObject; }

	void ShowGameplayMechanicsOnHud(const enum EUnlockInHud& WhatToUnlock);
	bool GetIsGameplayMechanicEnabled(const enum EUnlockInHud& WhatToGet);

private:

	UPROPERTY(EditAnywhere, Category = "Saves")
	TObjectPtr<class ASavedDataObject> SavedDataObject = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Saves")
	TSubclassOf<class UUserWidget> GameSavedNotificationWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Saves")
	TSubclassOf<class UUserWidget> CannotSavedNotificationWidgetClass = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Load")
	TSubclassOf<class AMarineCharacter> PlayerClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Load")
	TSubclassOf<class UUserWidget> GameLoadedWidgetClass = nullptr;

	void SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn);
	TObjectPtr<class USaveMarineRunner> CreateLoadGame();

	void SpawnNewPlayer();

	UPROPERTY(EditAnywhere, Category = "Gameplay Mechanics")
	bool bShowHealBar = false;
	UPROPERTY(EditAnywhere, Category = "Gameplay Mechanics")
	bool bShowDashBar = false;
	UPROPERTY(EditAnywhere, Category = "Gameplay Mechanics")
	bool bShowSlowMotionBar = false;

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarinePlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class UMarineRunnerGameInstance> GameInstance = nullptr;

};

