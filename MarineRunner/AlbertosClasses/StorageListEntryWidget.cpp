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
	if (ItemDataObject->bIsItEnoughToCraft)
	{
		MainImage->SetVisibility(ESlateVisibility::Visible);
		MainImageWithoutResources->SetVisibility(ESlateVisibility::Hidden);
		
		OutlineMat->SetVectorParameterValue(FName(TEXT("Color")), EnoughResourcesColor);
	}
	else
	{
		MainImage->SetVisibility(ESlateVisibility::Hidden);
		MainImageWithoutResources->SetVisibility(ESlateVisibility::Visible);

		OutlineMat->SetVectorParameterValue(FName(TEXT("Color")), NotEnoughResourcesColor);
	}
}
