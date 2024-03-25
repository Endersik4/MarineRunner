// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MessageToReadWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UMessageToReadWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;


public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> MessageTitle = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class URichTextBlock> MessageText = nullptr;
	UPROPERTY(EditAnywhere, Category = "Message To Read")
		TObjectPtr<USoundBase> OpenMessageSound = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> OpenMessageAnim = nullptr;

	void SetMessageInformation(const FText & _MessageTitle, const FText & _MessageText);
	void HideMessageAfterTime(float Time);
private:

	FTimerHandle HideMessageHandle;
	void HideMessage();
};
