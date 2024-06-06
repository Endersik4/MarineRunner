// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/Player/GameMenu/GameMenuBase.h"

#include "PauseMenuWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;

UCLASS()
class MARINERUNNER_API UPauseMenuWidget : public UGameMenuBase
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> BackgroundBlurImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> ResumeText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> ResumeButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UTextBlock> SaveGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr < UButton> SaveGameButton = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> ResumeHoverAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> SaveGameHoverAnim = nullptr;

	UFUNCTION()
	void OnClickedResumeButton();
	UFUNCTION()
	void OnHoveredResumeButton();
	UFUNCTION()
	void OnUnhoveredResumeButton();

	virtual void OnClickedLoadGameButton() override;

	UFUNCTION()
	void OnClickedSaveGameButton();
	UFUNCTION()
	void OnHoveredSaveGameButton();
	UFUNCTION()
	void OnUnhoveredSaveGameButton();

	virtual void OnClickedQuitGameButton() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Pause Menu Settings")
	TSubclassOf<class UConfirmOptionWidget> ConfirmOptionWidgetClass = nullptr;

	enum EMusicType MusicTypeBeforePause;
	void PlayPauseMenuMusic();
	void StopPauseMenuMusic();

	void QuitGame();

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};
