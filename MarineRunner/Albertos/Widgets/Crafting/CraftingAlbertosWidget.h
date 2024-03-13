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
		TSubclassOf<class UCraftedItemDataObject> ItemDataObject;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting Albertos Widget Settings")
		float FillCraftingPercentBarTimerTime = 0.001f;

	void AddItemsToInventoryTileView(const TArray<FItemStruct> & InventoryItems);
	void SwitchCurrentCraftingItem(bool bDeleteResources = false);

	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertos) { AlbertosPawn = NewAlbertos; }
	FORCEINLINE void SetPlayer(TObjectPtr<class AMarineCharacter> NewPlayer) { MarinePawn = NewPlayer; }
	FORCEINLINE void SetRecipesData(const TArray<FItemStruct> & NewRecipes) { RecipesOfCraftableItems = NewRecipes; }

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> BackgroundImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> LeftArrowButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> RightArrowButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_1x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_2x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_4x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_8x;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> AmountMultiplier_16x;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> CloseWidgetButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UButton> CraftButton;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> CraftingTimeProgressBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> CraftingItemImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> SecondCraftingItemImageForAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Right_SlideCraftingImagesAnim;
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Left_SlideCraftingImagesAnim;
	UFUNCTION()
		void OnSlideCraftingAnimFinished();

	// A place where all items will be stored except resources (first aid kits, ammunition...).
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> StorageInventoryTileView;

	// The Place where only Resources will be stored
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> ResourcesInventoryTileView;

	// The Place where it will be shown what Player needs to craft an item
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTileView> RequirementsInventoryTileView;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemNameToBeCraftedText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemDescriptionText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemTimeAndAmountText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemCraftTimeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemValue_AmountText;

	UFUNCTION()
		void CraftPressed();
	UFUNCTION()
		void CraftHovered();
	UFUNCTION()
		void CraftUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> CraftHoveredAnim;

	UFUNCTION()
		void LeftArrowClicked();
	UFUNCTION()
		void LeftArrowHovered();
	UFUNCTION()
		void LeftArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> LeftArrowHoveredAnim;

	UFUNCTION()
		void RightArrowClicked();
	UFUNCTION()
		void RightArrowHovered();
	UFUNCTION()
		void RightArrowUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  RightArrowHoveredAnim;

	UFUNCTION()
		void CloseWidgetButtonClicked();
	UFUNCTION()
		void CloseWidgetButtonHovered();
	UFUNCTION()
		void CloseWidgetButtonUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  CloseWidgetButtonHoveredAnim;

	UFUNCTION()
		void Multiplier_1xClicked();
	UFUNCTION()
		void Multiplier_1xHovered();
	UFUNCTION()
		void Multiplier_1xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  Multiplier_1xHoveredAnim;

	UFUNCTION()
		void Multiplier_2xClicked();
	UFUNCTION()
		void Multiplier_2xHovered();
	UFUNCTION()
		void Multiplier_2xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation>  Multiplier_2xHoveredAnim;

	UFUNCTION()
		void Multiplier_4xClicked();
	UFUNCTION()
		void Multiplier_4xHovered();
	UFUNCTION()
		void Multiplier_4xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_4xHoveredAnim;

	UFUNCTION()
		void Multiplier_8xClicked();
	UFUNCTION()
		void Multiplier_8xHovered();
	UFUNCTION()
		void Multiplier_8xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_8xHoveredAnim;

	UFUNCTION()
		void Multiplier_16xClicked();
	UFUNCTION()
		void Multiplier_16xHovered();
	UFUNCTION()
		void Multiplier_16xUnhovered();
	UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
		TObjectPtr < UWidgetAnimation> Multiplier_16xHoveredAnim;

	UFUNCTION()
		void OnItemInTileViewHovered(UObject* Item, bool bIsHovered);

private:

	TArray<FItemStruct> RecipesOfCraftableItems;

	int32 ChoiceOfCraftableItem = 0;
	int32 CraftingMultiplier = 1;
	
	// Crafting
	bool bCanUseCraftPanel = true;
	bool bCanCraftItem = true;
	void SetItemDataToUI(bool bDeleteResources);
	void AddItemResourcesToRequirementsList(bool bDeleteResources);
	bool DoesHaveEnoughResources(FString Resource, bool bDeleteResources = false);

	// Craft Button Progress Bar
	float CurrentItemCraftingTimeElapsed;
	float CurrentItemCraftTime;
	float ItemCraftTimeLeftInSeconds;
	FTimerHandle TimeCraftHandle;
	void SetPercentOfCraftingProgressBar();
	void SetCanUseCraftPanelAgain();

	// Swipe Items Icon Images Anim
	EChoiceOfArrow CurrentChoiceOfArrow = ECA_None;
	void PlaySwipeItemIconAnim();

	// Multiplier Buttons
	TObjectPtr<UButton> MultiplierButtonChoice;
	void MultiplierButtonClicked(int32 Mutliplier);
	void SetEnableAllMultipliers(bool bEnable);

	void (UCraftingAlbertosWidget::*CurrentMultiplierUnhoveredFunc)(void);

	void PlayButtonAnimation(UWidgetAnimation* AnimToPlay, bool bForward = true);

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn;
	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosPawn;
};
