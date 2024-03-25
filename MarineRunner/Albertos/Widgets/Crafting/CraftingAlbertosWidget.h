// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "CraftingAlbertosWidget.generated.h"

/**
 * A widget that appears when the player presses the action key on Albertos.
 * The widget stores everything the player takes.
 * The player can produce items using this widget, and the resource requirements of the items will appear in the ResourcesInventoryTileView
 * The resources that the player can take will appear in StorageInventoryTileView.
 */

UENUM()
enum EChoiceOfArrow
{
	ECA_Left,
	ECA_Right,
	ECA_None
};
class UButton; 
class UImage; 
class UTextBlock;
class UTileView;
class UProgressBar;

UCLASS()
class MARINERUNNER_API UCraftingAlbertosWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting Albertos Widget Settings")
		TSubclassOf<class UCraftedItemDataObject> ItemDataObject = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Albertos Widget Settings")
		float FillCraftingPercentBarTimerTime = 0.001f;

	void AddItemsToInventoryTileView(const TArray<FItemStruct> & InventoryItems);
	void SwitchCurrentCraftingItem(bool bDeleteResources = false);

	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertos) { AlbertosPawn = NewAlbertos; }
	FORCEINLINE void SetPlayer(TObjectPtr<class AMarineCharacter> NewPlayer) { MarinePawn = NewPlayer; }
	FORCEINLINE void SetRecipesData(const TArray<FItemStruct> & NewRecipes) { RecipesOfCraftableItems = NewRecipes; }

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> BackgroundImage = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> LeftArrowButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> RightArrowButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_1x = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_2x = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_4x = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_8x = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_16x = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> CloseWidgetButton = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> CraftButton = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> CraftingTimeProgressBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> CraftingItemImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> SecondCraftingItemImageForAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Right_SlideCraftingImagesAnim = nullptr;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Left_SlideCraftingImagesAnim = nullptr;
	UFUNCTION()
		void OnSlideCraftingAnimFinished();

	// A place where all items will be stored except resources (first aid kits, ammunition...).
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> StorageInventoryTileView = nullptr;

	// The Place where only Resources will be stored
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> ResourcesInventoryTileView = nullptr;

	// The Place where it will be shown what Player needs to craft an item
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> RequirementsInventoryTileView = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemNameToBeCraftedText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemDescriptionText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemTimeAndAmountText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemCraftTimeText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemValue_AmountText = nullptr;

	UFUNCTION()
		void CraftPressed();
	UFUNCTION()
		void CraftHovered();
	UFUNCTION()
		void CraftUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> CraftHoveredAnim = nullptr;

	UFUNCTION()
		void LeftArrowClicked();
	UFUNCTION()
		void LeftArrowHovered();
	UFUNCTION()
		void LeftArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> LeftArrowHoveredAnim = nullptr;

	UFUNCTION()
		void RightArrowClicked();
	UFUNCTION()
		void RightArrowHovered();
	UFUNCTION()
		void RightArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  RightArrowHoveredAnim = nullptr;

	UFUNCTION()
		void CloseWidgetButtonClicked();
	UFUNCTION()
		void CloseWidgetButtonHovered();
	UFUNCTION()
		void CloseWidgetButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  CloseWidgetButtonHoveredAnim = nullptr;

	UFUNCTION()
		void Multiplier_1xClicked();
	UFUNCTION()
		void Multiplier_1xHovered();
	UFUNCTION()
		void Multiplier_1xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  Multiplier_1xHoveredAnim = nullptr;

	UFUNCTION()
		void Multiplier_2xClicked();
	UFUNCTION()
		void Multiplier_2xHovered();
	UFUNCTION()
		void Multiplier_2xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  Multiplier_2xHoveredAnim = nullptr;

	UFUNCTION()
		void Multiplier_4xClicked();
	UFUNCTION()
		void Multiplier_4xHovered();
	UFUNCTION()
		void Multiplier_4xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_4xHoveredAnim = nullptr;

	UFUNCTION()
		void Multiplier_8xClicked();
	UFUNCTION()
		void Multiplier_8xHovered();
	UFUNCTION()
		void Multiplier_8xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_8xHoveredAnim = nullptr;

	UFUNCTION()
		void Multiplier_16xClicked();
	UFUNCTION()
		void Multiplier_16xHovered();
	UFUNCTION()
		void Multiplier_16xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_16xHoveredAnim = nullptr;

	UFUNCTION()
		void OnItemInTileViewHovered(UObject* Item, bool bIsHovered);

private:

	UPROPERTY(Transient)
		TArray<FItemStruct> RecipesOfCraftableItems;

	UPROPERTY(Transient)
		int32 ChoiceOfCraftableItem = 0;
	UPROPERTY(Transient)
		int32 CraftingMultiplier = 1;
	
	// Crafting
	UPROPERTY(Transient)
		bool bCanUseCraftPanel = true;
	UPROPERTY(Transient)
		bool bCanCraftItem = true;
	void SetItemDataToUI(bool bRefreshOnlyInventory);
	void AddItemResourcesToRequirementsList(bool bRefreshOnlyInventory);
	bool DoesHaveEnoughResources(const FString& ResourceName, bool bRefreshOnlyInventory = false);

	// Craft Button Progress Bar
	UPROPERTY(Transient)
		float CurrentItemCraftingTimeElapsed = 0.f;
	UPROPERTY(Transient)
		float CurrentItemCraftTime = 0.f;
	UPROPERTY(Transient)
		float ItemCraftTimeLeftInSeconds = 0.f;
	FTimerHandle TimeCraftHandle;
	void SetPercentOfCraftingProgressBar();
	void SetCanUseCraftPanelAgain();

	// Swipe Items Icon Images Anim
	UPROPERTY(Transient)
		TEnumAsByte<EChoiceOfArrow> CurrentChoiceOfArrow = ECA_None;
	void PlaySwipeItemIconAnim();

	// Multiplier Buttons
	UPROPERTY(Transient)
		TObjectPtr<UButton> MultiplierButtonChoice = nullptr;
	void MultiplierButtonClicked(int32 Mutliplier);
	void SetEnableAllMultipliers(bool bEnable);

	void (UCraftingAlbertosWidget::*CurrentMultiplierUnhoveredFunc)(void);

	void PlayButtonAnimation(UWidgetAnimation* AnimToPlay, bool bForward = true);

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosPawn = nullptr;
};
