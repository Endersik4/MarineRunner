// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/Widgets/Crafting/CraftingAlbertosWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Animation/WidgetAnimation.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Objects/Interactive/PickableItem.h"
#include "MarineRunner/Albertos/Objects/CraftedItemDataObject.h"
#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Albertos/Components/CraftItemAlbertosComponent.h"

void UCraftingAlbertosWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UCraftingAlbertosWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CraftButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::CraftPressed);
	CraftButton->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::CraftHovered);
	CraftButton->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::CraftUnhovered);

	LeftArrowButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::LeftArrowClicked);
	LeftArrowButton->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::LeftArrowHovered);
	LeftArrowButton->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::LeftArrowUnhovered);

	RightArrowButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::RightArrowClicked);
	RightArrowButton->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::RightArrowHovered);
	RightArrowButton->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::RightArrowUnhovered);

	CloseWidgetButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::CloseWidgetButtonClicked);
	CloseWidgetButton->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::CloseWidgetButtonHovered);
	CloseWidgetButton->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::CloseWidgetButtonUnhovered);

	AmountMultiplier_1x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_1xClicked);
	AmountMultiplier_1x->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_1xHovered);
	AmountMultiplier_1x->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_1xUnhovered);

	AmountMultiplier_2x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_2xClicked);
	AmountMultiplier_2x->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_2xHovered);
	AmountMultiplier_2x->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_2xUnhovered);

	AmountMultiplier_4x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_4xClicked);
	AmountMultiplier_4x->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_4xHovered);
	AmountMultiplier_4x->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_4xUnhovered);

	AmountMultiplier_8x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_8xClicked);
	AmountMultiplier_8x->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_8xHovered);
	AmountMultiplier_8x->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_8xUnhovered);

	AmountMultiplier_16x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_16xClicked);
	AmountMultiplier_16x->OnHovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_16xHovered);
	AmountMultiplier_16x->OnUnhovered.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_16xUnhovered);

	StorageInventoryTileView->OnItemIsHoveredChanged().AddUFunction(this, FName("OnItemInTileViewHovered"));
	ResourcesInventoryTileView->OnItemIsHoveredChanged().AddUFunction(this, FName("OnItemInTileViewHovered"));
	RequirementsInventoryTileView->OnItemIsHoveredChanged().AddUFunction(this, FName("OnItemInTileViewHovered"));

	FWidgetAnimationDynamicEvent SlideCraftingAnimFinished;
	SlideCraftingAnimFinished.BindDynamic(this, &UCraftingAlbertosWidget::OnSlideCraftingAnimFinished);
	BindToAnimationFinished(Left_SlideCraftingImagesAnim, SlideCraftingAnimFinished);
	BindToAnimationFinished(Right_SlideCraftingImagesAnim, SlideCraftingAnimFinished);

	Multiplier_1xHovered();
	Multiplier_1xClicked();
}

#pragma region /////////////////////// ADD DATA TO INVENTORY ///////////////////
void UCraftingAlbertosWidget::AddItemsToInventoryTileView(const TArray<FItemStruct> & InventoryItems)
{
	StorageInventoryTileView->ClearListItems();
	ResourcesInventoryTileView->ClearListItems();

	for (const FItemStruct& Items : InventoryItems)
	{
		if (!IsValid(ItemDataObject))
			continue;

		TObjectPtr<UCraftedItemDataObject> ConstructedItemObject = NewObject<UCraftedItemDataObject>(ItemDataObject);
		if (!IsValid(ConstructedItemObject))
			continue;

		ConstructedItemObject->ItemData = Items;

		if (Items.bIsItResource)
		{
			ResourcesInventoryTileView->AddItem(ConstructedItemObject);
		}
		else
		{
			StorageInventoryTileView->AddItem(ConstructedItemObject);
		}
	}
}
#pragma endregion

#pragma region ///////////////// item options /////////////////////
void UCraftingAlbertosWidget::SwitchCurrentCraftingItem(bool bRefreshOnlyInventory)
{
	if (!bCanUseCraftPanel || !IsValid(MarinePawn))
		return;

	RequirementsInventoryTileView->ClearListItems();

	if (RecipesOfCraftableItems.Num() < ChoiceOfCraftableItem) 
		return;

	if (!bRefreshOnlyInventory)
		SetItemDataToUI();

	PlaySwipeItemIconAnim();

	bCanCraftItem = true;
	CraftButton->SetIsEnabled(true);

	AddItemResourcesToRequirementsList(bRefreshOnlyInventory);
}

void UCraftingAlbertosWidget::SetItemDataToUI()
{
	if (CurrentChoiceOfArrow == ECA_None)
	{
		CraftingItemImage->SetBrushFromTexture(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_StorageIcon);
	}
	else if (RecipesOfCraftableItems.Num() > ChoiceOfCraftableItem)
	{
		SecondCraftingItemImageForAnim->SetBrushFromTexture(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_StorageIcon);
	}

	ItemNameToBeCraftedText->SetText(FText::FromString("-" + RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Name + "-"));
	ItemDescriptionText->SetText(FText::FromString(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Description));

	const FString TimeToCraftItem = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemCraftTimeText->SetText(FText::FromString(TimeToCraftItem));

	const FString AmountTextStr = FString::FromInt(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Amount * CraftingMultiplier);
	ItemValue_AmountText->SetText(FText::FromString(AmountTextStr));

	// Player cant craft multiple weapons at once
	if (RecipesOfCraftableItems[ChoiceOfCraftableItem].bIsItWeapon)
	{
		SetEnableAllMultipliers(false);
	}
	else
	{
		SetEnableAllMultipliers(true);
	}
}

void UCraftingAlbertosWidget::AddItemResourcesToRequirementsList(bool bRefreshOnlyInventory)
{
	for (const TPair<FString, int32> & CurrentResource : RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements)
	{
		const FItemStruct* ItemFoundInInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.FindByKey(CurrentResource.Key);
		if (!ItemFoundInInventory)
			continue;

		const bool HaveEnough = DoesHaveEnoughResources(CurrentResource.Key, bRefreshOnlyInventory);

		if (!IsValid(ItemDataObject))
			continue;

		TObjectPtr<UCraftedItemDataObject> ConstructedItemObject = NewObject<UCraftedItemDataObject>(ItemDataObject);
		if (!IsValid(ConstructedItemObject))
			continue;

		ConstructedItemObject->ItemData = *ItemFoundInInventory;
		ConstructedItemObject->ItemData.Item_Amount = CurrentResource.Value * CraftingMultiplier;
		ConstructedItemObject->bIsItEnoughToCraft = HaveEnough;

		RequirementsInventoryTileView->AddItem(ConstructedItemObject);

		if (bCanCraftItem && !HaveEnough) // Disable craft button if resource requirements are not met
		{
			bCanCraftItem = false;
			CraftButton->SetIsEnabled(false);
		}
	}
}

bool UCraftingAlbertosWidget::DoesHaveEnoughResources(const FString & ResourceName, bool bDeleteResources)
{
	FItemStruct* ResourceFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.FindByKey(ResourceName);
	if (!ResourceFromInventory) 
		return false;

	const int32* NumberOfResourcesNeeded = RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.Find(ResourceName);
	if (!NumberOfResourcesNeeded) 
		return false;

	if (ResourceFromInventory->Item_Amount < *NumberOfResourcesNeeded * CraftingMultiplier)
		return false;

	if (bDeleteResources)
	{
		ResourceFromInventory->Item_Amount -= *NumberOfResourcesNeeded * CraftingMultiplier;

		//if after deleting the amount from resource, the player doesnt have enough resource amount needed to create item then return false
		if (ResourceFromInventory->Item_Amount < *NumberOfResourcesNeeded * CraftingMultiplier) 
				return false;
	}

	return true;
}
#pragma endregion

void UCraftingAlbertosWidget::OnItemInTileViewHovered(UObject* Item, bool bIsHovered)
{
	if (!IsValid(MarinePawn)) 
		return;
	if (!IsValid(MarinePawn->GetHudWidget())) 
		return;

	if (bIsHovered == true)
	{
		if (!IsValid(Item))
			return;

		TObjectPtr<UCraftedItemDataObject> NewItem = Cast<UCraftedItemDataObject>(Item);
		if (!IsValid(NewItem))
			return;

		MarinePawn->GetHudWidget()->SetItemHoverInformations(NewItem->ItemData.Item_Name, NewItem->ItemData.Item_Description, NewItem->ItemData.Item_StorageIcon);
	}
	else
	{
		MarinePawn->GetHudWidget()->PlayAppearAnimForItemHover(false);
	}
}

#pragma region ///////////////// CRAFTING /////////////////////
void UCraftingAlbertosWidget::CraftPressed()
{
	if (!IsValid(MarinePawn) || !IsValid(AlbertosPawn))
		return;

	if (!bCanCraftItem || !bCanUseCraftPanel)
		return;

	SwitchCurrentCraftingItem(true);
	MarinePawn->UpdateAlbertosInventory();

	AlbertosPawn->GetCraftItemAlbertosComponent()->CraftPressed(MarinePawn, &RecipesOfCraftableItems[ChoiceOfCraftableItem], CraftingMultiplier);

	CraftButton->SetIsEnabled(false);
	LeftArrowButton->SetIsEnabled(false);
	RightArrowButton->SetIsEnabled(false);
	CraftingTimeProgressBar->SetVisibility(ESlateVisibility::Visible);

	CurrentItemCraftingTimeElapsed = 0.f;
	ItemCraftTimeLeftInSeconds = RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft;

	// Set Can craft again after TimeCraft
	bCanUseCraftPanel = false;
	CurrentItemCraftTime = RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft;
	GetWorld()->GetTimerManager().SetTimer(TimeCraftHandle, this, &UCraftingAlbertosWidget::SetPercentOfCraftingProgressBar, FillCraftingPercentBarTimerTime, true);
}

void UCraftingAlbertosWidget::CraftHovered()
{
	PlayButtonAnimation(CraftHoveredAnim);
}

void UCraftingAlbertosWidget::CraftUnhovered()
{
	PlayButtonAnimation(CraftHoveredAnim, false);
}

void UCraftingAlbertosWidget::SetPercentOfCraftingProgressBar()
{
	if (bCanUseCraftPanel)
		return;

	if (CurrentItemCraftingTimeElapsed <= CurrentItemCraftTime)
	{
		const float& CraftingTimeProgress = FMath::Lerp(0.f, 1.f, CurrentItemCraftingTimeElapsed / CurrentItemCraftTime);
		CraftingTimeProgressBar->SetPercent(CraftingTimeProgress);

		const FString SecondsLeftString = FString::SanitizeFloat(FMath::RoundValuesToGivenDecimalNumbers(ItemCraftTimeLeftInSeconds, 2)) + "s";
		ItemCraftTimeText->SetText(FText::FromString(SecondsLeftString));

		ItemCraftTimeLeftInSeconds -= FillCraftingPercentBarTimerTime;
		CurrentItemCraftingTimeElapsed += FillCraftingPercentBarTimerTime;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimeCraftHandle);
		SetCanUseCraftPanelAgain();
	}
}

void UCraftingAlbertosWidget::SetCanUseCraftPanelAgain()
{
	CraftingTimeProgressBar->SetPercent(0.f);
	CraftingTimeProgressBar->SetVisibility(ESlateVisibility::Hidden);

	LeftArrowButton->SetIsEnabled(true);
	RightArrowButton->SetIsEnabled(true);

	AlbertosPawn->GetCraftItemAlbertosComponent()->CraftingFinished();

	const FString Time = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemCraftTimeText->SetText(FText::FromString(Time));

	bCanUseCraftPanel = true;
	CraftButton->SetIsEnabled(true);
}
#pragma endregion

#pragma region /////////////////// Swipe Items Icons Images Animation //////////////////////
void UCraftingAlbertosWidget::PlaySwipeItemIconAnim()
{
	if (CurrentChoiceOfArrow == ECA_Left)
	{
		PlayAnimationForward(Left_SlideCraftingImagesAnim, 1.f, true);
	}
	else if (CurrentChoiceOfArrow == ECA_Right)
	{
		PlayAnimationForward(Right_SlideCraftingImagesAnim, 1.f, true);
	}
	CurrentChoiceOfArrow = ECA_None;
}

void UCraftingAlbertosWidget::OnSlideCraftingAnimFinished()
{
	CraftingItemImage->SetBrushFromTexture(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_StorageIcon);
}

#pragma region ///////////////////////////// CHOICE - ARROWS //////////////////////////////
void UCraftingAlbertosWidget::LeftArrowClicked()
{
	CurrentChoiceOfArrow = ECA_Left;

	if (ChoiceOfCraftableItem > 0)
	{
		ChoiceOfCraftableItem--;
	}
	else if (ChoiceOfCraftableItem == 0)
	{
		ChoiceOfCraftableItem = RecipesOfCraftableItems.Num() - 1;
	}
	else
	{
		return;
	}

	if (MultiplierButtonChoice != AmountMultiplier_1x)
	{
		Multiplier_1xHovered();
	}
	Multiplier_1xClicked();
}

void UCraftingAlbertosWidget::LeftArrowHovered()
{
	PlayButtonAnimation(LeftArrowHoveredAnim);
}

void UCraftingAlbertosWidget::LeftArrowUnhovered()
{
	PlayButtonAnimation(LeftArrowHoveredAnim, false);
}

void UCraftingAlbertosWidget::RightArrowClicked()
{
	CurrentChoiceOfArrow = ECA_Right;

	if (ChoiceOfCraftableItem < RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem++;
	}
	else if (ChoiceOfCraftableItem == RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem = 0;
	}
	else
	{
		return;
	}

	if (MultiplierButtonChoice != AmountMultiplier_1x)
	{
		Multiplier_1xHovered();
	}
	Multiplier_1xClicked();
}

void UCraftingAlbertosWidget::RightArrowHovered()
{
	PlayButtonAnimation(RightArrowHoveredAnim);
}

void UCraftingAlbertosWidget::RightArrowUnhovered()
{
	PlayButtonAnimation(RightArrowHoveredAnim, false);
}

#pragma endregion

void UCraftingAlbertosWidget::CloseWidgetButtonClicked()
{
	if (!IsValid(AlbertosPawn))
		return;

	AlbertosPawn->ToggleInventoryVisibility();
}

void UCraftingAlbertosWidget::CloseWidgetButtonHovered()
{
	PlayButtonAnimation(CloseWidgetButtonHoveredAnim);
}

void UCraftingAlbertosWidget::CloseWidgetButtonUnhovered()
{
	PlayButtonAnimation(CloseWidgetButtonHoveredAnim, false);
}

#pragma region //////////////////////////////// Mutlipliers Buttons/////////////////////////////////

void UCraftingAlbertosWidget::MultiplierButtonClicked(int32 Mutliplier)
{	
	CraftingMultiplier = Mutliplier;
	SwitchCurrentCraftingItem();

	if (CurrentMultiplierUnhoveredFunc)
	{
		(this->*(CurrentMultiplierUnhoveredFunc))();
	}
}

void UCraftingAlbertosWidget::SetEnableAllMultipliers(bool bEnable)
{
	AmountMultiplier_2x->SetIsEnabled(bEnable);
	AmountMultiplier_4x->SetIsEnabled(bEnable);
	AmountMultiplier_8x->SetIsEnabled(bEnable);
	AmountMultiplier_16x->SetIsEnabled(bEnable);
}

void UCraftingAlbertosWidget::Multiplier_1xClicked()
{
	if (!bCanUseCraftPanel) 
		return;

	MultiplierButtonChoice = AmountMultiplier_1x;
	MultiplierButtonClicked(1);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_1xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_1xHovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_1x)
		return;

	PlayButtonAnimation(Multiplier_1xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_1xUnhovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_1x)
		return;

	PlayButtonAnimation(Multiplier_1xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_2xClicked()
{
	if (!bCanUseCraftPanel) 
		return;

	MultiplierButtonChoice = AmountMultiplier_2x;
	MultiplierButtonClicked(2);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_2xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_2xHovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_2x)
		return;

	PlayButtonAnimation(Multiplier_2xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_2xUnhovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_2x)
		return;

	PlayButtonAnimation(Multiplier_2xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_4xClicked()
{
	if (!bCanUseCraftPanel) 
		return;

	MultiplierButtonChoice = AmountMultiplier_4x;
	MultiplierButtonClicked(4);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_4xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_4xHovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_4x)
		return;

	PlayButtonAnimation(Multiplier_4xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_4xUnhovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_4x)
		return;

	PlayButtonAnimation(Multiplier_4xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_8xClicked()
{
	if (!bCanUseCraftPanel) 
		return;

	MultiplierButtonChoice = AmountMultiplier_8x;
	MultiplierButtonClicked(8);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_8xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_8xHovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_8x)
		return;

	PlayButtonAnimation(Multiplier_8xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_8xUnhovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_8x)
		return;

	PlayButtonAnimation(Multiplier_8xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_16xClicked()
{
	if (!bCanUseCraftPanel) 
		return;

	MultiplierButtonChoice = AmountMultiplier_16x;
	MultiplierButtonClicked(16);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_16xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_16xHovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_16x)
		return;

	PlayButtonAnimation(Multiplier_16xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_16xUnhovered()
{
	if (MultiplierButtonChoice == AmountMultiplier_16x)
		return;

	PlayButtonAnimation(Multiplier_16xHoveredAnim, false);
}

void UCraftingAlbertosWidget::PlayButtonAnimation(UWidgetAnimation* AnimToPlay, bool bForward)
{
	if (!AnimToPlay)
		return;

	if (bForward)
	{
		PlayAnimationForward(AnimToPlay);
	}
	else
	{
		PlayAnimationReverse(AnimToPlay);
	}
}

#pragma endregion