// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YouDiedWidget.generated.h"


class UButton;
UCLASS()
class MARINERUNNER_API UYouDiedWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> RestartGameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> QuitGameText = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> RestartGameButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> QuitGameButton = nullptr;

	UFUNCTION()
		void RestartGameButton_OnClicked();
	UFUNCTION()
		void RestartGameButton_OnHovered();
	UFUNCTION()
		void RestartGameButton_OnUnhovered();

	UFUNCTION()
		void QuitButton_OnClicked();
	UFUNCTION()
		void QuitButton_OnHovered();
	UFUNCTION()
		void QuitButton_OnUnhovered();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ShowWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> RestartGameHoveredAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> QuitHoveredAnim = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Death")
		TObjectPtr<USoundBase> DeathSound = nullptr;

	void PlayAnimatonForButton(TObjectPtr<UWidgetAnimation> AnimToPlay, bool bPlayForwardAnim = true);
	void RestartGame();
};
