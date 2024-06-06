// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "LoadSaveInformationData.h"

#include "LoadSaveEntry.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API ULoadSaveEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UImage> ScreenshotSaveImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> SaveNameText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> SaveDateText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> TotalTimeText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UButton> LoadGameButton = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr < UWidgetAnimation> LoadGameHoverAnim = nullptr;

	UFUNCTION()
	void OnClickedLoadGameButton();
	UFUNCTION()
	void OnHoveredLoadGameButton();
	UFUNCTION()
	void OnUnhoveredLoadGameButton();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
	bool bShowConfirmLoadingWidget = true;
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings", meta = (EditCondition = "bShowConfirmLoadingWidget"))
	TSubclassOf<class UConfirmOptionWidget> ConfirmOptionWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
	FString SavedDateText = "-SAVED: ";
	UPROPERTY(EditDefaultsOnly, Category = "Saved Data Settings")
	FString SavedTotalTimeText = "-TOTAL TIME: ";

	UPROPERTY(Transient)
	TObjectPtr<class ULoadSaveEntryObject> LoadGameEntryObject = nullptr;
	void ShowConfirmLoadingWidget();

	void LoadSave();

	void ConvertTotalPlayTimeInSecondsToText();
	void SetScreenshotImageFromSave();
};
