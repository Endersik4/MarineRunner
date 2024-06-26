// Copyright Adam Bartela.All Rights Reserved
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
	TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameSavedText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> GameSavedAppearAnim = nullptr;

private:
	UFUNCTION()
	void GameSavedAnimFinished();
};
