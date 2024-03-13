// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventorySlotEntryWidget.generated.h"

/**
 * List Entry for display an item with Item Icon and Item Amount
 */
UCLASS()
class MARINERUNNER_API UInventorySlotEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UButton> ItemButton;
	UFUNCTION()
		void ItemButtonHovered();
	UFUNCTION()
		void ItemButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ItemHoveredAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DisabledItemHoveredAnim;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> ItemImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> ItemAmountTextBlock;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DisableItemAnim;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Storage List Entry")
		FString TextWhenItemAmountIsMoreThen999 = "+999";

	bool bNotEnoughResources = false;
	void ItemDataToUI(TObjectPtr<class UCraftedItemDataObject> ItemDataObject);
};
