// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/Player/GameMenu/GameMenuBase.h"

#include "MainMenuWidget.generated.h"

UCLASS()
class MARINERUNNER_API UMainMenuWidget : public UGameMenuBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> ContinueText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> ContinueButton = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ContinueHoverAnim = nullptr;
	UFUNCTION()
	void OnClickedContinueButton();
	UFUNCTION()
	void OnHoveredContinueButton();
	UFUNCTION()
	void OnUnhoveredContinueButton();

	virtual void OnClickedLoadGameButton() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> NewGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> NewGameButton = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> NewGameHoverAnim = nullptr;
	UFUNCTION()
	void OnClickedNewGameButton();
	UFUNCTION()
	void OnHoveredNewGameButton();
	UFUNCTION()
	void OnUnhoveredNewGameButton();

	virtual void OnClickedQuitGameButton() override;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HidePauseWidgetAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeOutWidgetAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> FadeInWidgetAnim = nullptr;

	// Returns all .json files from save game folder
	UFUNCTION(BlueprintImplementableEvent)
	void GetJsonFilesFromSaveFolder(TArray<FString>& Json_FilesPath);

	UFUNCTION()
	void OnAnimFinished_FadeInWidget();

	///ZTGK
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class URichTextBlock> ZTGKInformationRichText = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowZTGKInformationAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideZTGKInformationAnim = nullptr;

public:
	void ShowZTGKInformation(bool bShow = true);

	void MainMusicFadeOut();
	void StartNewGame();
	FORCEINLINE void SetMainMenuPawn(TObjectPtr<class AMainMenuPawn> NewMainMenuPawn) { MainMenuPawn = NewMainMenuPawn; }

private:

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
	FName NewGameLevelName = "L_GameInformationMap";
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
	float MainMenuMusicFadeIn = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu Settings")
	float MainMenuMusicFadeOut = 3.f;
	void MainMenuMusic();


	// If there are no saves then hide continue button
	void HideContinueButton();
	virtual void RemoveLoadGameMenuWidgetFromViewport(bool bUnhoverTextLoadGame = false) override;

	// Continue Game
	UPROPERTY(Transient)
	TArray<FString> Json_SaveFilesPath;
	struct FSaveDataMenuStruct GetLastSavedSaveGame();

	FTimerHandle SpawnInformationHandle;
	// Fade out when starting/loading game
	FTimerHandle StartGameHandle;
	void ContinueGame();

	UPROPERTY(Transient)
	TObjectPtr<class AMainMenuPawn> MainMenuPawn = nullptr;
};
