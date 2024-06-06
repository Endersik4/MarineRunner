// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectDifficultyWidget.generated.h"


class UImage;
class UButton;
class UTextBlock;
class UMainMenuWidget;
UCLASS()
class MARINERUNNER_API USelectDifficultyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UImage> BackgroundImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideWidgetAnim = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentDifficultyLevelText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> LeftArrowChoiceButton = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> LeftArrowChoiceHoverAnim = nullptr;
	UFUNCTION()
	void OnClickedLeftChoiceButton();
	UFUNCTION()
	void OnHoveredLeftChoiceButton();
	UFUNCTION()
	void OnUnhoveredLeftChoiceButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> RightArrowChoiceButton = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> RightArrowChoiceHoverAnim = nullptr;
	UFUNCTION()
	void OnClickedRightChoiceButton();
	UFUNCTION()
	void OnHoveredRightChoiceButton();
	UFUNCTION()
	void OnUnhoveredRightChoiceButton();

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<UButton> StartNewGameButton = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> StartNewGameHoverAnim = nullptr;
	UFUNCTION()
	void OnClickedStartNewGameButton();
	UFUNCTION()
	void OnHoveredStartNewGameButton();
	UFUNCTION()
	void OnUnhoveredStartNewGameButton();

public:
	TFunction<void()>CallNewGameFunc;
	TFunction<void()>CallMainMenuMusicFadeOutFunc;
private:
	void PlayAnimatonForButton(TObjectPtr < UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true);

	UFUNCTION()
	void OnAnimFinished_HideWidget();

};
