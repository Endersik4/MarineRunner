// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "StorageListEntryWidget.generated.h"

/**
 * List Entry for display an item with Item Icon and Item Amount
 */
UCLASS()
class MARINERUNNER_API UStorageListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor EnoughResourcesColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor NotEnoughResourcesColor;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* MainImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* MainImageWithoutResources;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* StorageItemButton;
	UFUNCTION()
		void StorageItemButtonHovered();
	UFUNCTION()
		void StorageItemButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* StorageItemButtonHoveredAnim;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ItemImageListEntry;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemAmountListEntry;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Storage List Entry")
		FString TextWhenItemAmountIsMoreThen999 = "+999";
};
