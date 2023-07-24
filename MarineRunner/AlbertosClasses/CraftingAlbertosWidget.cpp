// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/AlbertosClasses/ItemDataObject.h"
#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/GunClasses/Gun.h"

void UCraftingAlbertosWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CraftButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::CraftPressed);

	LeftArrowButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::LeftArrowClicked);
	RightArrowButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::RightArrowClicked);

	AmountMultiplier_1x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_1xClicked);
	AmountMultiplier_2x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_2xClicked);
	AmountMultiplier_4x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_4xClicked);
	AmountMultiplier_8x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_8xClicked);
	AmountMultiplier_16x->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::Multiplier_16xClicked);
}

void UCraftingAlbertosWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MultiplierChoice = AmountMultiplier_1x;
	UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

void UCraftingAlbertosWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	SetPercentOfCraftingProgressBar(DeltaTime);
}

#pragma region /////////////////////// ADD DATE TO INVENTORY ///////////////////
void UCraftingAlbertosWidget::SetRecipesData()
{
	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (MarinePawn == nullptr) return;
	RecipesOfCraftableItems.Empty();

	for (TSubclassOf<APickupItem> PickableItem : MarinePawn->GetInventoryComponent()->Recipes_Items)
	{
		APickupItem* SpawnedItem = GetWorld()->SpawnActor<APickupItem>(PickableItem, FVector(0.f), FRotator(0.f));
		if (SpawnedItem == nullptr) continue;

		RecipesOfCraftableItems.Add(SpawnedItem->GetItemSettings());
		SpawnedItem->Destroy();
	}
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
		ConstructedItemObject->ConditionalBeginDestroy();
	}
}
#pragma endregion

#pragma region ///////////////// CRAFTING /////////////////////
void UCraftingAlbertosWidget::SwitchCurrentCraftingItem(bool bDeleteResources)
{
	if (bCanCraft == false || MarinePawn == nullptr) return;
	RequirementsInventoryTileView->ClearListItems();
	if (RecipesOfCraftableItems.Num() < ChoiceOfCraftableItem) return;

	SetVisualDataFromItemToUI(bDeleteResources);

	bCanBeCrafted = true;
	CraftButton->SetIsEnabled(true);

	AddResourcesToVisualRequirements(bDeleteResources);
}

void UCraftingAlbertosWidget::SetVisualDataFromItemToUI(bool bDeleteResources)
{
	if (bDeleteResources == true) return;

	CraftingItemImage->SetBrushFromTexture(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_StorageIcon);
	ItemNameToBeCraftedText->SetText(FText::FromString(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Name));
	ItemDescriptionText->SetText(FText::FromString(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Description));
	FString Time = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemValue_TimeText->SetText(FText::FromString(Time));
	ItemValue_AmountText->SetText(FText::FromString(FString::FromInt(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Amount)));

	if (RecipesOfCraftableItems[ChoiceOfCraftableItem].bIsItWeapon == true) SetisEnableAllMultipliers(false);
	else SetisEnableAllMultipliers(true);
}

void UCraftingAlbertosWidget::AddResourcesToVisualRequirements(bool bDeleteResources)
{
	TArray<FString> ResourcesName;
	RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.GenerateKeyArray(ResourcesName);
	for (FString NameOfResource : ResourcesName)
	{
		if (MarinePawn->GetInventoryComponent()->Inventory_Items.Find(NameOfResource) == nullptr) continue;

		bool HaveEnough = DoesHaveEnoughResources(NameOfResource, bDeleteResources);

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

void UCraftingAlbertosWidget::CraftPressed()
{
	if (MarinePawn == nullptr || bCanCraft == false) return;

	if (bCanBeCrafted == false || AlbertosPawn == nullptr) return;

	APickupItem* SpawnedItem;
	FVector SpawnLocation = AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(FName(TEXT("ItemSpawnLocation")));
	FRotator SpawnRotation = AlbertosPawn->GetActorRotation() + RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_CraftRotation;
	if (RecipesOfCraftableItems[ChoiceOfCraftableItem].bIsItWeapon == true)
	{
		SpawnedItem = GetWorld()->SpawnActor<AGun>(MarinePawn->GetInventoryComponent()->Recipes_Items[ChoiceOfCraftableItem], SpawnLocation, SpawnRotation);
	}
	else SpawnedItem = GetWorld()->SpawnActor<APickupItem>(MarinePawn->GetInventoryComponent()->Recipes_Items[ChoiceOfCraftableItem], SpawnLocation, SpawnRotation);
	if (SpawnedItem == nullptr) return;

	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel1, ECR_Ignore);
	SpawnedItem->SetCollisionNewResponse(ECC_GameTraceChannel3, ECR_Ignore);

	AlbertosPawn->CraftPressed(SpawnedItem);

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

	// Set Can craft again after TimeCraft
	bCanCraft = false;
	WaitTime = SpawnedItem->GetItemSettings().Item_TimeCraft;
	GetWorld()->GetTimerManager().SetTimer(TimeCraftHandle, this, &UCraftingAlbertosWidget::SetCanCraftAgain, WaitTime, false);
}

void UCraftingAlbertosWidget::SetPercentOfCraftingProgressBar(float Delta)
{
	if (bCanCraft == true) return;

	if (TimeElapsed <= WaitTime)
	{
		float Progress = FMath::Lerp(0.f, 1.f, TimeElapsed / WaitTime);

		CraftingTimeProgressBar->SetPercent(Progress);
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

	bCanCraft = true;
	CraftButton->SetIsEnabled(true);
}


#pragma endregion

#pragma region ///////////////////////////// CHOICE - ARROWS //////////////////////////////
void UCraftingAlbertosWidget::LeftArrowClicked()
{
	if (ChoiceOfCraftableItem > 0)
	{
		ChoiceOfCraftableItem--;
	}
	else if (ChoiceOfCraftableItem == 0)
	{
		ChoiceOfCraftableItem = RecipesOfCraftableItems.Num() - 1;
	}
	else return;

	Multiplier_1xClicked();
}

void UCraftingAlbertosWidget::RightArrowClicked()
{
	if (ChoiceOfCraftableItem < RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem++;
	}
	else if (ChoiceOfCraftableItem == RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem = 0;
	}
	else return;

	Multiplier_1xClicked();
}
#pragma endregion

#pragma region //////////////////////////////// Mutlipliers Buttons/////////////////////////////////

void UCraftingAlbertosWidget::MultiplierClicked(int32 Mutliplier)
{
	if (bCanCraft == false) return;

	if (MultiplierChoice && ChoiceOfMultiplierChoice < OriginalMultiplierChoiceTextures.Num())
	{
		UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, OriginalMultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
	}
	
	CraftingMultiplier = Mutliplier;
	SwitchCurrentCraftingItem();
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
	MultiplierClicked(1);
	ChoiceOfMultiplierChoice = 0;
	MultiplierChoice = AmountMultiplier_1x;
	if (ChoiceOfMultiplierChoice < MultiplierChoiceTextures.Num()) UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

void UCraftingAlbertosWidget::Multiplier_2xClicked()
{
	MultiplierClicked(2);
	MultiplierChoice = AmountMultiplier_2x;
	ChoiceOfMultiplierChoice = 1;
	if (ChoiceOfMultiplierChoice < MultiplierChoiceTextures.Num()) UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

void UCraftingAlbertosWidget::Multiplier_4xClicked()
{
	MultiplierClicked(4);
	MultiplierChoice = AmountMultiplier_4x;
	ChoiceOfMultiplierChoice = 2;
	if (ChoiceOfMultiplierChoice < MultiplierChoiceTextures.Num()) UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

void UCraftingAlbertosWidget::Multiplier_8xClicked()
{
	MultiplierClicked(8);
	MultiplierChoice = AmountMultiplier_8x;
	ChoiceOfMultiplierChoice = 3;
	if (ChoiceOfMultiplierChoice < MultiplierChoiceTextures.Num()) UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

void UCraftingAlbertosWidget::Multiplier_16xClicked()
{
	MultiplierClicked(16);
	MultiplierChoice = AmountMultiplier_16x;
	ChoiceOfMultiplierChoice = 4;
	if (ChoiceOfMultiplierChoice < MultiplierChoiceTextures.Num()) UWidgetBlueprintLibrary::SetBrushResourceToTexture(MultiplierChoice->WidgetStyle.Normal, MultiplierChoiceTextures[ChoiceOfMultiplierChoice]);
}

#pragma endregion