// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SaveLoadPlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	FORCEINLINE TObjectPtr<class ASavedDataObject> GetSavedDataObject() {return SavedDataObject;}

	void ShowGameplayMechanicsOnHud(const enum EUnlockInHud& WhatToUnlock);
private:

	UPROPERTY(EditAnywhere, Category = "Saves")
		TObjectPtr<class ASavedDataObject> SavedDataObject;
	UPROPERTY(EditDefaultsOnly, Category = "Saves")
		TSubclassOf<class UUserWidget> GameSavedNotificationWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Saves")
		TSubclassOf<class UUserWidget> CannotSavedNotificationWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Load")
		TSubclassOf<class AMarineCharacter> PlayerClass;
	UPROPERTY(EditDefaultsOnly, Category = "Load")
		TSubclassOf<class UUserWidget> NewGameBeginsWidgetClass;

	void SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn);
	TObjectPtr<class USaveMarineRunner> CreateLoadGame();

	void SpawnNewPlayer();

	bool bShowHealBar;
	bool bShowDashBar;
	bool bShowSlowMotionBar;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
	UPROPERTY(Transient)
		TObjectPtr<class AMarinePlayerController> PlayerController;
	UPROPERTY(Transient)
		TObjectPtr<class UMarineRunnerGameInstance> GameInstance;

};

