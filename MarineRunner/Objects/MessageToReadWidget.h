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
		class UTextBlock* MessageTitle;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* MessageText;
	UPROPERTY(EditAnywhere, Category = "Settings")
		USoundBase* OpenMessageSound;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* OpenMessageAnim = nullptr;

	void SetMessageInformation(const FText & _MessageTitle, const FText & _MessageText);

	void HideMessageAfterTime(float Time);
private:

	FTimerHandle HideMessageHandle;
	void HideMessage();

};
