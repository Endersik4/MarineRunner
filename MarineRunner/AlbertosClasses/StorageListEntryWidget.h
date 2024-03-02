// Copyright Adam Bartela.All Rights Reserved

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

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* ItemButton;
	UFUNCTION()
		void ItemButtonHovered();
	UFUNCTION()
		void ItemButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* ItemHoveredAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* DisabledItemHoveredAnim;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ItemImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemAmountTextBlock;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* DisableItemAnim;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Storage List Entry")
		FString TextWhenItemAmountIsMoreThen999 = "+999";

	bool bNotEnoughResources = false;
	void ItemDataToUI(class UItemDataObject* ItemDataObject);
};
