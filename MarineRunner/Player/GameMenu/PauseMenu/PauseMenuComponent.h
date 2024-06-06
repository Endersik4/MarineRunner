// Copyright Adam Bartela. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PauseMenuComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UPauseMenuComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPauseMenuComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	FORCEINLINE TObjectPtr<class UPauseMenuWidget> GetPauseMenuWidget() const { return PauseMenuWidget; }
	FORCEINLINE const bool& GetIsInPauseMenu() const { return bIsInPauseMenu; }

	void PauseGame();
	void UnPauseGame();

	void ChangeUIToGameOnly();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu")
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass = nullptr;

	UPROPERTY(Transient)
	bool bIsInPauseMenu = false;
	void SpawnPauseMenuWidget();
	bool CanPauseGame();
	UPROPERTY(Transient)
	TObjectPtr<class UPauseMenuWidget> PauseMenuWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};