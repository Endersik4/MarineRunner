// Fill out your copyright notice in the Description page of Project Settings.

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
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void GetTextFilesFromSaves(TArray<FString> & Txt_Files);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* NoSavedDataText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UListView* SavesListView;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Load Game Menu")
		TSubclassOf<class ULoadGameMenuEntryObject> MenuSettingsDataObject;

private:
	void FillSavesListView();

};
