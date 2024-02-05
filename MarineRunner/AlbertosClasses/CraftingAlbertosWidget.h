// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "CraftingAlbertosWidget.generated.h"

/**
 * A widget that appears when the player presses the action key on Albertos.
 * The widget stores everything the player takes.
 * The player can produce items using this widget, and the resource requirements of the items will appear in the ResourcesInventoryTileView
 * The resources that the player can take will appear in StorageInventoryTileView.
 */

enum EChoiceOfArrow
{
	ECA_Left,
	ECA_Right,
	ECA_None
};
UCLASS()
class MARINERUNNER_API UCraftingAlbertosWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UItemDataObject> ItemDataObject;

	void AddItemToTileView(TArray<struct FItemStruct> InventoryItems);
	void SetRecipesData(class AMarineCharacter* Player);
	void SwitchCurrentCraftingItem(bool bDeleteResources = false);

	void SetAlbertosPawn(class AAlbertosPawn* NewAlbertos) { AlbertosPawn = NewAlbertos; }

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* LeftArrowButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* RightArrowButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* AmountMultiplier_1x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* AmountMultiplier_2x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* AmountMultiplier_4x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* AmountMultiplier_8x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* AmountMultiplier_16x;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UButton* CraftButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* CraftingTimeProgressBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* CraftingItemImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* SecondCraftingItemImageForAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Right_SlideCraftingImagesAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Left_SlideCraftingImagesAnim;
	UFUNCTION()
		void OnSlideCraftingAnimFinished();

	// A place where all items will be stored except resources (first aid kits, ammunition...).
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* StorageInventoryTileView;

	// The Place where only Resources will be stored
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* ResourcesInventoryTileView;

	UFUNCTION(BlueprintCallable)
		void OnItemHovered(UObject* Item, bool bIsHovered);

	// The Place where it will be shown what Player needs to craft an item
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* RequirementsInventoryTileView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemNameToBeCraftedText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemDescriptionText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemTimeAndAmountText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemCraftTimeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemValue_AmountText;

	UFUNCTION()
		void CraftPressed();
	UFUNCTION()
		void CraftHovered();
	UFUNCTION()
		void CraftUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* CraftHoveredAnim;

	UFUNCTION()
		void LeftArrowClicked();
	UFUNCTION()
		void LeftArrowHovered();
	UFUNCTION()
		void LeftArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* LeftArrowHoveredAnim;

	UFUNCTION()
		void RightArrowClicked();
	UFUNCTION()
		void RightArrowHovered();
	UFUNCTION()
		void RightArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* RightArrowHoveredAnim;

	UFUNCTION()
		void Multiplier_1xClicked();
	UFUNCTION()
		void Multiplier_1xHovered();
	UFUNCTION()
		void Multiplier_1xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Multiplier_1xHoveredAnim;

	UFUNCTION()
		void Multiplier_2xClicked();
	UFUNCTION()
		void Multiplier_2xHovered();
	UFUNCTION()
		void Multiplier_2xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Multiplier_2xHoveredAnim;

	UFUNCTION()
		void Multiplier_4xClicked();
	UFUNCTION()
		void Multiplier_4xHovered();
	UFUNCTION()
		void Multiplier_4xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Multiplier_4xHoveredAnim;

	UFUNCTION()
		void Multiplier_8xClicked();
	UFUNCTION()
		void Multiplier_8xHovered();
	UFUNCTION()
		void Multiplier_8xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Multiplier_8xHoveredAnim;

	UFUNCTION()
		void Multiplier_16xClicked();
	UFUNCTION()
		void Multiplier_16xHovered();
	UFUNCTION()
		void Multiplier_16xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation* Multiplier_16xHoveredAnim;

private:

	TArray<FItemStruct> RecipesOfCraftableItems;

	int32 ChoiceOfCraftableItem = 0;
	int32 CraftingMultiplier = 1;
	
	// Crafting
	bool bCanCraft = true;
	bool bCanCraftItem = true;
	void SetItemDataToUI(bool bDeleteResources);
	void AddItemResourcesToRequirementsList(bool bDeleteResources);
	bool DoesHaveEnoughResources(FString Resource, bool bDeleteResources = false);

	// Craft Button Progress Bar
	float TimeElapsed;
	float WaitTime;
	float CopiedItemCraftTime;
	FTimerHandle TimeCraftHandle;
	void SetPercentOfCraftingProgressBar();
	void SetCanCraftAgain();

	// Swipe Items Icon Images Anim
	EChoiceOfArrow CurrentChoiceOfArrow = ECA_None;
	void PlayProperSwipeItemIconAnim();

	// Multiplier Buttons
	class UButton* MultiplierChoice;
	void MultiplierClicked(int32 Mutliplier);
	void SetisEnableAllMultipliers(bool bEnable);

	void (UCraftingAlbertosWidget::*CurrentMultiplierUnhoveredFunc)(void);

	void PlayButtonAnimation(UWidgetAnimation* AnimToPlay, bool bForward = true);

	class AMarineCharacter* MarinePawn;
	class AAlbertosPawn* AlbertosPawn;
};
