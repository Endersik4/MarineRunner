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

	FORCEINLINE void SetSavedDataObject(class ASavedDataObject* NewSavedDataObject) { SavedDataObject = NewSavedDataObject; }
	FORCEINLINE class ASavedDataObject* GetSavedDataObject() {return SavedDataObject;}


private:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> GameSavedNotificationWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CannotSavedNotificationWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> NewGameBeginsWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Saves")
		class ASavedDataObject* SavedDataObject;
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TSubclassOf<class AMarineCharacter> PlayerClass;

	//Saving/Loading Game
	class USaveMarineRunner* CreatedSaveGame;
	void SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn);

	void SpawnNewPlayer();

	class AMarineCharacter* Player;
	class AMarinePlayerController* PlayerController;
	class UMarineRunnerGameInstance* GameInstance;

};

