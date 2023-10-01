// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LoadGameData.h"

#include "SaveGameMenuListEntry.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API USaveGameMenuListEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ScreenshotSaveImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SaveNameText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* SaveDateText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* TotalTimeText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* LoadGameButton;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* LoadGameHoverAnim = nullptr;

	UFUNCTION()
		void OnClickedLoadGameButton();
	UFUNCTION()
		void OnHoveredLoadGameButton();
	UFUNCTION()
		void OnUnhoveredLoadGameButton();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
		TSubclassOf<UUserWidget> ConfirmLoadingSaveWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
		FString SavedDateText = "-SAVED: ";
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
		FString SavedTotalTimeText = "-TOTAL TIME: ";

	void PlayAnimatonForButton(UWidgetAnimation* AnimToPlay, bool bPlayForwardAnim = true);

	class ULoadGameMenuEntryObject* ListEntryObject;

	void ConvertTotalPlayTimeInSecondsToText();
	void SetScreenshotImageFromSave();
};
