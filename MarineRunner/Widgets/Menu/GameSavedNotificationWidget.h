// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameSavedNotificationWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UGameSavedNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* GameSavedText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* GameSavedAppearAnim = nullptr;

private:
	UFUNCTION()
		void GameSavedAnimFinished();
};
