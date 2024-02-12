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
		class UImage* BackgroundImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* RestartGameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* QuitGameText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* RestartGameButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UButton* QuitGameButton;

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
		UWidgetAnimation* ShowWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* RestartGameHoveredAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* QuitHoveredAnim = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* DeathSound;
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
		TSubclassOf<class AMarineCharacter> PlayerClass;

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true);
	void RestartGame();
};
