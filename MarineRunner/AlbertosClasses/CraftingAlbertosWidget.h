// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CraftingAlbertosWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UCraftingAlbertosWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class UItemDataObject> ItemDataObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UTexture2D*> MultiplierChoiceTextures;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<UTexture2D*> OriginalMultiplierChoiceTextures;


	void AddDataToList(TArray<struct FItemStruct> InventoryItems);
	void SetRecipesData();
	void SwitchCurrentCraftingItem(bool bDeleteResources = false);

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

	// A place where all items will be stored except resources (first aid kits, ammunition...).
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* StorageInventoryTileView;

	// The Place where only Resources will be stored
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTileView* ResourcesInventoryTileView;

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
		class UTextBlock* ItemValue_TimeText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemValue_AmountText;

	UFUNCTION()
		void CraftClicked();

	UFUNCTION()
		void LeftArrowClicked();
	UFUNCTION()
		void RightArrowClicked();

	UFUNCTION()
		void Multiplier_1xClicked();
	UFUNCTION()
		void Multiplier_2xClicked();
	UFUNCTION()
		void Multiplier_4xClicked();
	UFUNCTION()
		void Multiplier_8xClicked();
	UFUNCTION()
		void Multiplier_16xClicked();

private:

	TArray<struct FItemStruct> RecipesOfCraftableItems;

	int32 ChoiceOfCraftableItem = 0;
	int32 CraftingMultiplier = 1;
	bool bCanCraft = true;

	bool bCanBeCrafted;
	bool DoesHaveEnoughResources(FString Resource, bool bDeleteResources = false);


	float TimeElapsed;
	float WaitTime;
	void SetPercentOfCraftingProgressBar(float Delta);
	FTimerHandle TimeCraftHandle;
	void CanCraftAgain();

	int32 ChoiceOfMultiplierChoice = 0;
	class UButton* MultiplierChoice;
	void MultiplierClicked(int32 Mutliplier);
	void SetisEnableAllMultipliers(bool bEnable);

	class AMarineCharacter* MarinePawn;

};
