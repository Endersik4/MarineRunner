// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBase.generated.h"

class UTextBlock;
class UButton;
class UImage;

struct FVisiblePauseMenu
{
	TObjectPtr<UUserWidget> VisibleWidget = nullptr;
	TFunction<void(bool)> FunctionToHideWidget = nullptr;

	FVisiblePauseMenu(TObjectPtr<UUserWidget> _VisibleWidget, TFunction<void(bool)> _FunctionToHideWidget)
	{
		VisibleWidget = _VisibleWidget;
		FunctionToHideWidget = _FunctionToHideWidget;
	}

	bool operator==(const TObjectPtr<UUserWidget>& CompareToVisibleWidget)
	{
		return VisibleWidget == CompareToVisibleWidget;
	}
};

UCLASS()
class MARINERUNNER_API UMenuBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> MarineRunnerTitleImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> LoadGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> LoadGameButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> SettingsText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> SettingsButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> QuitGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> QuitGameButton = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> ShowPauseWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> LoadGameHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> SettingsHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> QuitGameHoverAnim = nullptr;

	UFUNCTION()
	virtual void OnClickedLoadGameButton();
	UFUNCTION()
	virtual void OnHoveredLoadGameButton();
	UFUNCTION()
	virtual void OnUnhoveredLoadGameButton();

	UFUNCTION()
	virtual void OnClickedSettingsButton();
	UFUNCTION()
	virtual void OnHoveredSettingsButton();
	UFUNCTION()
	virtual void OnUnhoveredSettingsButton();

	UFUNCTION()
	virtual void OnClickedQuitGameButton();
	UFUNCTION()
	virtual void OnHoveredQuitGameButton();
	UFUNCTION()
	virtual void OnUnhoveredQuitGameButton();
public:
	// returns true if there is no more left active menu widgets
	bool BackToPreviousMenu();
	TArray<FVisiblePauseMenu> CurrentSpawnedMenuWidgets;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
	TObjectPtr < USoundBase> PauseMenuMusic = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Load Game Menu")
	TSubclassOf<class ULoadGameMenuWidget> LoadGameMenuWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Settings Menu")
	TSubclassOf<class USettingsMenuWidget> SettingsMenuWidgetClass = nullptr;

	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true, bool bCanHoverGivenText = false);

	UPROPERTY(Transient)
	bool bWasLoadGameMenuWidgetSpawned = false;
	void SpawnLoadGameMenuWidget();
	void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false);
	UPROPERTY(Transient)
	TObjectPtr<class ULoadGameMenuWidget> LoadGameMenuWidget = nullptr;

	UPROPERTY(Transient)
	bool bWasSettingsMenuWidgetSpawned = false;
	void SpawnSettingsMenuWidget();
	void RemoveSettingsMenuWidgetFromViewport(bool bUnhoverTextSettings = false);
	UPROPERTY(Transient)
	TObjectPtr<class USettingsMenuWidget> SettingsMenuWidget = nullptr;

	void AddAllMenuButtonsToArray();
	UPROPERTY(Transient)
	TArray< TObjectPtr<UButton>> AllMenuButtons;
	void EnableAllMenuButtons(bool bEnable, TObjectPtr<UButton> ButtonToIgnore = nullptr);

	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class UMarineRunnerGameInstance> GameInstance = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<	class UAudioComponent> CurrentMenuMusic = nullptr;
};
