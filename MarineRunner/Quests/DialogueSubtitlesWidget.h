// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueSubtitlesWidget.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API UDialogueSubtitlesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;
public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class URichTextBlock> CurrentSubtitlesRichText = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> ShowWidgetAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> HideWidgetAnim = nullptr;

	void ShowWidget(bool bShow);

	void ChangeCurrentSubtitles(const FText& NewTextForSubtitles);
};
