// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/StorageListEntryWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "MarineRunner/AlbertosClasses/ItemDataObject.h"

void UStorageListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStorageListEntryWidget::NativeOnInitialized()
{

}

void UStorageListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UItemDataObject* ItemDataObject = Cast<UItemDataObject>(ListItemObject);

	ItemImageListEntry->SetBrushFromTexture(ItemDataObject->ItemData.Item_StorageIcon);
	int32 ItemAmountValue = ItemDataObject->ItemData.Item_Amount;
	if (ItemAmountValue > 999) ItemAmountValue = 999;

	ItemAmountListEntry->SetText(FText::FromString(FString::FromInt(ItemAmountValue)));

	UMaterialInstanceDynamic* OutlineMat = ItemAmountListEntry->GetDynamicOutlineMaterial();

	MainImage->SetVisibility(ItemDataObject->bIsItEnoughToCraft ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	MainImageWithoutResources->SetVisibility(ItemDataObject->bIsItEnoughToCraft ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	
	OutlineMat->SetVectorParameterValue(FName(TEXT("Color")), ItemDataObject->bIsItEnoughToCraft ? EnoughResourcesColor : NotEnoughResourcesColor);
}
