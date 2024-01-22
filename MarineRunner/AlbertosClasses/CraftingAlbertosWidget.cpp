// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Animation/WidgetAnimation.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/AlbertosClasses/ItemDataObject.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"

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

	FWidgetAnimationDynamicEvent SlideCraftingAnimFinished;
	SlideCraftingAnimFinished.BindDynamic(this, &UCraftingAlbertosWidget::OnSlideCraftingAnimFinished);
	BindToAnimationFinished(Left_SlideCraftingImagesAnim, SlideCraftingAnimFinished);
	BindToAnimationFinished(Right_SlideCraftingImagesAnim, SlideCraftingAnimFinished);

	Multiplier_1xHovered();
	Multiplier_1xClicked();
}

void UCraftingAlbertosWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	SetPercentOfCraftingProgressBar(DeltaTime);
}

#pragma region /////////////////////// ADD DATA TO INVENTORY ///////////////////
void UCraftingAlbertosWidget::SetRecipesData(AMarineCharacter* Player)
{
	if (IsValid(Player) == false)
		return;

	MarinePawn = Player;
	RecipesOfCraftableItems = Player->GetInventoryComponent()->Items_Recipes;
}

void UCraftingAlbertosWidget::AddItemToTileView(TArray<FItemStruct> InventoryItems)
{
	StorageInventoryTileView->ClearListItems();
	ResourcesInventoryTileView->ClearListItems();

	for (FItemStruct Items : InventoryItems)
	{
		UItemDataObject* ConstructedItemObject = NewObject<UItemDataObject>(ItemDataObject);
		ConstructedItemObject->ItemData = Items;

		if (Items.bIsItResource == true) ResourcesInventoryTileView->AddItem(ConstructedItemObject);
		else StorageInventoryTileView->AddItem(ConstructedItemObject);
	}
}
#pragma endregion

#pragma region ///////////////// CRAFTING /////////////////////
void UCraftingAlbertosWidget::SwitchCurrentCraftingItem(bool bRefreshInventory)
{
	if (bCanCraft == false || MarinePawn == nullptr) 
		return;

	RequirementsInventoryTileView->ClearListItems();

	if (RecipesOfCraftableItems.Num() < ChoiceOfCraftableItem) 
		return;

	SetItemDataToUI(bRefreshInventory);

	PlayProperSwipeItemIconAnim();

	bCanBeCrafted = true;
	CraftButton->SetIsEnabled(true);

	AddItemResourcesToRequirementsList(bRefreshInventory);
}

void UCraftingAlbertosWidget::SetItemDataToUI(bool bRefreshInventory)
{
	if (bRefreshInventory == true) 
		return;

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

	FString Time = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemValue_TimeText->SetText(FText::FromString(Time));

	FString AmountTextStr = FString::FromInt(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Amount * CraftingMultiplier);
	ItemValue_AmountText->SetText(FText::FromString(AmountTextStr));

	if (RecipesOfCraftableItems[ChoiceOfCraftableItem].bIsItWeapon == true) SetisEnableAllMultipliers(false);
	else SetisEnableAllMultipliers(true);
}

void UCraftingAlbertosWidget::AddItemResourcesToRequirementsList(bool bRefreshInventory)
{
	TArray<FString> ResourcesName;
	RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.GenerateKeyArray(ResourcesName);
	for (FString NameOfResource : ResourcesName)
	{
		if (MarinePawn->GetInventoryComponent()->Inventory_Items.Find(NameOfResource) == nullptr) continue;

		bool HaveEnough = DoesHaveEnoughResources(NameOfResource, bRefreshInventory);

		UItemDataObject* ConstructedItemObject = NewObject<UItemDataObject>(ItemDataObject);
		ConstructedItemObject->ItemData = *MarinePawn->GetInventoryComponent()->Inventory_Items.Find(NameOfResource);
		ConstructedItemObject->ItemData.Item_Amount = (*RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.Find(NameOfResource) * CraftingMultiplier);
		ConstructedItemObject->bIsItEnoughToCraft = HaveEnough;
		RequirementsInventoryTileView->AddItem(ConstructedItemObject);

		if (bCanBeCrafted == true && HaveEnough == false)
		{
			bCanBeCrafted = false;
			CraftButton->SetIsEnabled(false);
		}

		ConstructedItemObject->ConditionalBeginDestroy();
	}
}

bool UCraftingAlbertosWidget::DoesHaveEnoughResources(FString Resource, bool bDeleteResources)
{
	FItemStruct* ResourceFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(Resource);
	if (ResourceFromInventory == nullptr) return false;

	int32* NumberOfResourcesNeeded = RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.Find(Resource);
	if (NumberOfResourcesNeeded == nullptr) return false;

	if (ResourceFromInventory->Item_Amount >= *NumberOfResourcesNeeded * CraftingMultiplier)
	{
		if (bDeleteResources == true)
		{
			ResourceFromInventory->Item_Amount -= *NumberOfResourcesNeeded * CraftingMultiplier;
			if (!(ResourceFromInventory->Item_Amount >= *NumberOfResourcesNeeded * CraftingMultiplier)) return false;
		}
	}
	else return false;

	return true;
}

void UCraftingAlbertosWidget::OnItemHovered(UObject* Item, bool bIsHovered)
{
	if (MarinePawn == nullptr) return;
	if (MarinePawn->GetHudWidget() == nullptr) return;

	if (bIsHovered == true)
	{
		UItemDataObject* NewItem = Cast<UItemDataObject>(Item);
		MarinePawn->GetHudWidget()->SetItemHoverInformations(NewItem->ItemData.Item_Name, NewItem->ItemData.Item_Description, NewItem->ItemData.Item_StorageIcon);
	}
	else MarinePawn->GetHudWidget()->PlayAppearAnimForItemHover(false);
}

void UCraftingAlbertosWidget::CraftPressed()
{
	if (MarinePawn == nullptr || bCanCraft == false) return;

	if (bCanBeCrafted == false || AlbertosPawn == nullptr) return;

	APickupItem* SpawnedItem;
	FVector SpawnLocation = AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(FName(TEXT("ItemSpawnLocation")));
	SpawnLocation += AlbertosPawn->GetActorForwardVector() * RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftLocation.X;
	SpawnLocation += AlbertosPawn->GetActorRightVector() * RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftLocation.Y;

	FRotator SpawnRotation = AlbertosPawn->GetActorRotation() + RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftRotation;
	SpawnedItem = GetWorld()->SpawnActor<APickupItem>(RecipesOfCraftableItems[ChoiceOfCraftableItem].ItemObject, SpawnLocation, SpawnRotation);
	if (SpawnedItem == nullptr) return;

	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel1, ECR_Ignore);
	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel3, ECR_Ignore);

	AlbertosPawn->CraftPressed(SpawnedItem, &TimeCraftHandle);

	// Refresh Inventory
	SpawnedItem->SetItemAmount(SpawnedItem->GetItemSettings().Item_Amount * CraftingMultiplier);
	SwitchCurrentCraftingItem(true);
	MarinePawn->UpdateAlbertosInventory();

	// Effects
	CraftButton->SetIsEnabled(false);
	LeftArrowButton->SetIsEnabled(false);
	RightArrowButton->SetIsEnabled(false);
	CraftingTimeProgressBar->SetVisibility(ESlateVisibility::Visible);

	// Fill in Progress bar
	TimeElapsed = 0.f;
	CopiedItemCraftTime = SpawnedItem->GetItemSettings().Item_TimeCraft;

	// Set Can craft again after TimeCraft
	bCanCraft = false;
	WaitTime = SpawnedItem->GetItemSettings().Item_TimeCraft;
	GetWorld()->GetTimerManager().SetTimer(TimeCraftHandle, this, &UCraftingAlbertosWidget::SetCanCraftAgain, WaitTime, false);
}

void UCraftingAlbertosWidget::CraftHovered()
{
	PlayButtonAnimation(CraftHoveredAnim);

}

void UCraftingAlbertosWidget::CraftUnhovered()
{
	PlayButtonAnimation(CraftHoveredAnim, false);
}

void UCraftingAlbertosWidget::SetPercentOfCraftingProgressBar(float Delta)
{
	if (bCanCraft == true) 
		return;

	if (TimeElapsed <= WaitTime)
	{
		float Progress = FMath::Lerp(0.f, 1.f, TimeElapsed / WaitTime);
		CraftingTimeProgressBar->SetPercent(Progress);

		FString TimeInSeconds = FString::SanitizeFloat(FMath::RoundValuesToGivenDecimalNumbers(CopiedItemCraftTime, 2)) + "s";
		ItemValue_TimeText->SetText(FText::FromString(TimeInSeconds));
		
		CopiedItemCraftTime -= Delta;
		TimeElapsed += Delta;
	}
}

void UCraftingAlbertosWidget::SetCanCraftAgain()
{
	CraftingTimeProgressBar->SetPercent(0.f);
	CraftingTimeProgressBar->SetVisibility(ESlateVisibility::Hidden);

	LeftArrowButton->SetIsEnabled(true);
	RightArrowButton->SetIsEnabled(true);

	AlbertosPawn->CraftingFinished();

	FString Time = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemValue_TimeText->SetText(FText::FromString(Time));

	bCanCraft = true;
	CraftButton->SetIsEnabled(true);
}


#pragma endregion

#pragma region /////////////////// Swipe Items Icons Images Animation //////////////////////
void UCraftingAlbertosWidget::PlayProperSwipeItemIconAnim()
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
	else return;

	if (MultiplierChoice != AmountMultiplier_1x)
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
	else return;

	if (MultiplierChoice != AmountMultiplier_1x)
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

#pragma region //////////////////////////////// Mutlipliers Buttons/////////////////////////////////

void UCraftingAlbertosWidget::MultiplierClicked(int32 Mutliplier)
{	
	CraftingMultiplier = Mutliplier;
	SwitchCurrentCraftingItem();

	if (CurrentMultiplierUnhoveredFunc)
	{
		(this->*(CurrentMultiplierUnhoveredFunc))();
	}
}

void UCraftingAlbertosWidget::SetisEnableAllMultipliers(bool bEnable)
{
	AmountMultiplier_2x->SetIsEnabled(bEnable);
	AmountMultiplier_4x->SetIsEnabled(bEnable);
	AmountMultiplier_8x->SetIsEnabled(bEnable);
	AmountMultiplier_16x->SetIsEnabled(bEnable);
}

void UCraftingAlbertosWidget::Multiplier_1xClicked()
{
	if (bCanCraft == false) return;

	MultiplierChoice = AmountMultiplier_1x;
	MultiplierClicked(1);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_1xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_1xHovered()
{
	if (MultiplierChoice == AmountMultiplier_1x)
		return;

	PlayButtonAnimation(Multiplier_1xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_1xUnhovered()
{
	if (MultiplierChoice == AmountMultiplier_1x)
		return;

	PlayButtonAnimation(Multiplier_1xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_2xClicked()
{
	if (bCanCraft == false) return;

	MultiplierChoice = AmountMultiplier_2x;
	MultiplierClicked(2);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_2xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_2xHovered()
{
	if (MultiplierChoice == AmountMultiplier_2x)
		return;

	PlayButtonAnimation(Multiplier_2xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_2xUnhovered()
{
	if (MultiplierChoice == AmountMultiplier_2x)
		return;

	PlayButtonAnimation(Multiplier_2xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_4xClicked()
{
	if (bCanCraft == false) return;

	MultiplierChoice = AmountMultiplier_4x;
	MultiplierClicked(4);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_4xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_4xHovered()
{
	if (MultiplierChoice == AmountMultiplier_4x)
		return;

	PlayButtonAnimation(Multiplier_4xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_4xUnhovered()
{
	if (MultiplierChoice == AmountMultiplier_4x)
		return;

	PlayButtonAnimation(Multiplier_4xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_8xClicked()
{
	if (bCanCraft == false) return;

	MultiplierChoice = AmountMultiplier_8x;
	MultiplierClicked(8);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_8xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_8xHovered()
{
	if (MultiplierChoice == AmountMultiplier_8x)
		return;

	PlayButtonAnimation(Multiplier_8xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_8xUnhovered()
{
	if (MultiplierChoice == AmountMultiplier_8x)
		return;

	PlayButtonAnimation(Multiplier_8xHoveredAnim, false);
}

void UCraftingAlbertosWidget::Multiplier_16xClicked()
{
	if (bCanCraft == false) return;

	MultiplierChoice = AmountMultiplier_16x;
	MultiplierClicked(16);
	CurrentMultiplierUnhoveredFunc = &UCraftingAlbertosWidget::Multiplier_16xUnhovered;
}

void UCraftingAlbertosWidget::Multiplier_16xHovered()
{
	if (MultiplierChoice == AmountMultiplier_16x)
		return;

	PlayButtonAnimation(Multiplier_16xHoveredAnim);
}

void UCraftingAlbertosWidget::Multiplier_16xUnhovered()
{
	if (MultiplierChoice == AmountMultiplier_16x)
		return;

	PlayButtonAnimation(Multiplier_16xHoveredAnim, false);
}

void UCraftingAlbertosWidget::PlayButtonAnimation(UWidgetAnimation* AnimToPlay, bool bForward)
{
	if (AnimToPlay == nullptr)
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