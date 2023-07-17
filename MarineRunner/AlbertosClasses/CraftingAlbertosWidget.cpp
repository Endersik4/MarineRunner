// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/AlbertosClasses/CraftingAlbertosWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/TileView.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/AlbertosClasses/ItemDataObject.h"
#include "MarineRunner/GunClasses/Gun.h"

void UCraftingAlbertosWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CraftButton->OnClicked.AddDynamic(this, &UCraftingAlbertosWidget::CraftClicked);

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
}

void UCraftingAlbertosWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	SetPercentOfCraftingProgressBar(DeltaTime);
}

void UCraftingAlbertosWidget::AddDataToList(TArray<FItemStruct> InventoryItems)
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

void UCraftingAlbertosWidget::SwitchCurrentCraftingItem()
{
	RequirementsInventoryTileView->ClearListItems();
	if (RecipesOfCraftableItems.Num() < ChoiceOfCraftableItem || MarinePawn == nullptr) return;

	CraftingItemImage->SetBrushFromTexture(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_StorageIcon);
	ItemNameToBeCraftedText->SetText(FText::FromString(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Name));
	ItemDescriptionText->SetText(FText::FromString(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Description));
	FString Time = FString::SanitizeFloat(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_TimeCraft) + "s";
	ItemValue_TimeText->SetText(FText::FromString(Time));
	ItemValue_AmountText->SetText(FText::FromString(FString::FromInt(RecipesOfCraftableItems[ChoiceOfCraftableItem].Item_Amount)));

	TArray<FString> ResourcesName;
	RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.GenerateKeyArray(ResourcesName);
	for (int32 i = 0; i != ResourcesName.Num(); i++)
	{
		if (MarinePawn->GetInventoryComponent()->Inventory_Items.Find(ResourcesName[i]) == nullptr) continue;

		UItemDataObject* ConstructedItemObject = NewObject<UItemDataObject>(ItemDataObject);
		ConstructedItemObject->ItemData = *MarinePawn->GetInventoryComponent()->Inventory_Items.Find(ResourcesName[i]);
		ConstructedItemObject->ItemData.Item_Amount = (*RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.Find(ResourcesName[i]) * CraftingMultiplier);
		RequirementsInventoryTileView->AddItem(ConstructedItemObject);

		ConstructedItemObject->ConditionalBeginDestroy();
	}
}

void UCraftingAlbertosWidget::CraftClicked()
{
	if (MarinePawn == nullptr || bCanCraft == false) return;

	bool bCanBeCrafted = DoesHaveEnoughResources();
	if (bCanBeCrafted == true)
	{
		UE_LOG(LogTemp, Error, TEXT("HAVE RESOURCES"));
		APickupItem* SpawnedItem;
		if (RecipesOfCraftableItems[ChoiceOfCraftableItem].bIsItWeapon == true)
		{
			SpawnedItem = GetWorld()->SpawnActor<AGun>(MarinePawn->GetInventoryComponent()->Recipes_Items[ChoiceOfCraftableItem], FVector(0.f), FRotator(0.f));
		}
		else SpawnedItem = GetWorld()->SpawnActor<APickupItem>(MarinePawn->GetInventoryComponent()->Recipes_Items[ChoiceOfCraftableItem], FVector(0.f), FRotator(0.f));
		if (SpawnedItem == nullptr) return;
		SpawnedItem->SetItemAmount(SpawnedItem->GetItemSettings().Item_Amount * CraftingMultiplier);
		MarinePawn->UpdateAlbertosInventory();

		bCanCraft = false;
		TimeElapsed = 0.f;
		WaitTime = SpawnedItem->GetItemSettings().Item_TimeCraft;
		GetWorld()->GetTimerManager().SetTimer(TimeCraftHandle, this, &UCraftingAlbertosWidget::CanCraftAgain, WaitTime, false);
	}
	else UE_LOG(LogTemp, Error, TEXT("DOESNOT HAVE RESOURCES"));
}

bool UCraftingAlbertosWidget::DoesHaveEnoughResources()
{
	TArray<FString> ResourcesName;
	RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.GenerateKeyArray(ResourcesName);
	for (int32 i = 0; i != ResourcesName.Num(); i++)
	{
		FItemStruct* ResourceFromInventory = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(ResourcesName[i]);
		if (ResourceFromInventory == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NIE ZNALEZIONO PRZEDMIOTU %s"), *ResourcesName[i]);
			return false;
		}
		
		int32* NumberOfResourcesNeeded = RecipesOfCraftableItems[ChoiceOfCraftableItem].ResourceRequirements.Find(ResourcesName[i]);
		if (NumberOfResourcesNeeded == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("NIE ZNALEZIONO ILOSCi"));
			return false;
		}
		

		if (ResourceFromInventory->Item_Amount >= *NumberOfResourcesNeeded * CraftingMultiplier) ResourceFromInventory->Item_Amount -= *NumberOfResourcesNeeded * CraftingMultiplier;
		else
		{
			return false;
			UE_LOG(LogTemp, Warning, TEXT("ZA MALO %s"), *ResourceFromInventory->Item_Name);
		}
	}
	return true;
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

void UCraftingAlbertosWidget::LeftArrowClicked()
{
	if (bCanCraft == false) return;

	if (ChoiceOfCraftableItem > 0)
	{
		ChoiceOfCraftableItem--;
	}
	else if (ChoiceOfCraftableItem == 0)
	{
		ChoiceOfCraftableItem = RecipesOfCraftableItems.Num() - 1;
	}
	else return;

	CraftingMultiplier = 1;
	SwitchCurrentCraftingItem();
}

void UCraftingAlbertosWidget::RightArrowClicked()
{
	if (bCanCraft == false) return;

	if (ChoiceOfCraftableItem < RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem++;
	}
	else if (ChoiceOfCraftableItem == RecipesOfCraftableItems.Num() - 1)
	{
		ChoiceOfCraftableItem = 0;
	}
	else return;

	CraftingMultiplier = 1;
	SwitchCurrentCraftingItem();
}

void UCraftingAlbertosWidget::CanCraftAgain()
{
	CraftingTimeProgressBar->SetPercent(0.f);
	bCanCraft = true;
}

void UCraftingAlbertosWidget::MultiplierClicked(int32 Mutliplier)
{
	if (bCanCraft == false) return;

	CraftingMultiplier = Mutliplier;
	SwitchCurrentCraftingItem();
}

void UCraftingAlbertosWidget::Multiplier_1xClicked()
{
	MultiplierClicked(1);
}

void UCraftingAlbertosWidget::Multiplier_2xClicked()
{
	MultiplierClicked(2);
}

void UCraftingAlbertosWidget::Multiplier_4xClicked()
{
	MultiplierClicked(4);
}

void UCraftingAlbertosWidget::Multiplier_8xClicked()
{
	MultiplierClicked(8);
}

void UCraftingAlbertosWidget::Multiplier_16xClicked()
{
	MultiplierClicked(16);
}