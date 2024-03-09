// Copyright Adam Bartela.All Rights Reserved


#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadGameData.h"

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
	UFUNCTION(BlueprintImplementableEvent)
		void GetTextFilesFromSaves(TArray<FString> & Txt_Files);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> NoSavedDataText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UListView> LoadGamesListView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Load Game Menu")
		TSubclassOf<class ULoadGameMenuEntryObject> MenuSettingsDataObject;

private:
	void FillLoadGamesListView();

	// Get all the values from json file and make struct of it (FSaveDataMenuStruct) and add it to array;
	void FillDeserializedSaveFilesToArray(TArray<FString> & PathsToSaveFiles,TArray<FSaveDataMenuStruct>& ArrayToFill);

	// Creates LoadGameMenuEntryObject from Array adds to the list
	void ConvertArrayToLoadGameMenuEntryList(TArray<FSaveDataMenuStruct>& ArrayToConvert);
};
