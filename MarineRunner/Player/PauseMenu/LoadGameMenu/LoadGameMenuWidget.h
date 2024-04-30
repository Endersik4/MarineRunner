// Copyright Adam Bartela.All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveInformationData.h"

#include "LoadGameMenuWidget.generated.h"

/**
 * 
 */

UCLASS()
class MARINERUNNER_API ULoadGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> NoSavedDataText = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UListView> SavedGameSavesListView = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> DeleteAllSavesButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> DeleteAllSavesText = nullptr;

	UFUNCTION()
		void DeleteAllSaves_OnClicked();
	UFUNCTION()
		void DeleteAllSaves_OnHovered();
	UFUNCTION()
		void DeleteAllSaves_OnUnhovered();

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> DeleteAllSavesHoveredAnim = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Load Game Menu")
		TSubclassOf<class ULoadGameMenuEntryObject> SavedGameSaveEntry = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Load Game Menu")
		TSubclassOf<class UConfirmOptionWidget> ConfirmDeletingSavesWidgetClass = nullptr;
private:
	void FillAllSavesToLoadGameListView();

	// Get all the values from json file and make struct of it (FSaveDataMenuStruct) and add it to array;
	void FillDeserializedSaveFilesToArray(TArray<FString> & PathsToSaveFiles,TArray<FSaveDataMenuStruct>& ArrayToFill);

	// Creates LoadGameMenuEntryObject from Array adds to the list
	void ConvertArrayToLoadGameMenuEntryList(TArray<FSaveDataMenuStruct>& ArrayToConvert);

	void DeleteAllSaves();
	void SavesVisible(bool bHide = false);
};
