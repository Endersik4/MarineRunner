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

	void CallSaveGame(AActor* JustSavedCheckpoint = nullptr) { SaveGame(JustSavedCheckpoint); }
	bool CanPlayerSaveGame();
	void SpawnCannotSavedWidget() { SpawnPassingWidget(CannotSavedNotificationWidgetClass); }

	void LoadSavedSettingsFromGameInstance();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> GameSavedNotificationWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CannotSavedNotificationWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Saves")
		class ASavedDataObject* SavedDataObject;

	//Saving/Loading Game
	class USaveMarineRunner* CreatedSaveGame;
	void SaveGame(AActor* JustSavedCheckpoint = nullptr);
	void LoadGame();
	void SpawnPassingWidget(const TSubclassOf<class UUserWidget>& WidgetClassToSpawn);
		
	class AMarineCharacter* Player;
	class AMarinePlayerController* PlayerController;
	class UMarineRunnerGameInstance* GameInstance;

};

