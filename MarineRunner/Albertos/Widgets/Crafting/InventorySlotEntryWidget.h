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
		TObjectPtr<class UButton> ItemButton = nullptr;
	UFUNCTION()
		void ItemButtonHovered();
	UFUNCTION()
		void ItemButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ItemHoveredAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DisabledItemHoveredAnim = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UImage> ItemImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<class UTextBlock> ItemAmountTextBlock = nullptr;

	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> DisableItemAnim = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Storage List Entry")
		int32 MaxAmountToDisplay = 999;
	UPROPERTY(EditDefaultsOnly, Category = "Storage List Entry")
		FString ExceededMaxAmountToDisplayText = "+999";

	UPROPERTY(Transient)
		bool bNotEnoughResources = false;

	void ItemDataToUI(TObjectPtr<class UCraftedItemDataObject> ItemDataObject);
};
