// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidget.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API UCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void ChangeCrosshair(TObjectPtr<UTexture2D> NewCrosshairTexture);
	void ShowCrosshairWithAnim(bool bShow = true);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> CrosshairImage = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowCrosshairWidget = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideCrosshairWidget = nullptr;
};
