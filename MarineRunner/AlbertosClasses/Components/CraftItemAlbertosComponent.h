// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Inventory/InventoryComponent.h"

#include "CraftItemAlbertosComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UCraftItemAlbertosComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftItemAlbertosComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CraftPressed(class AMarineCharacter* Player, const FItemStruct* ItemToCraft, int32 ItemAmountMultiplier);
	void CraftingFinished();

	FORCEINLINE const bool isCraftedItemValid() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float ItemMoveSpeedAfterCrafting = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float ItemScaleSpeedAfterCrafting = 11.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		FName ItemSpawnLocationSocketName = TEXT("ItemSpawnLocation");
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		FName FinalItemLocationSocketName = TEXT("FinalItemPosition");
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		UMaterialInstance* OverlayCraftingMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* Craft_Start_Sound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* Craft_Middle_Sound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* Craft_End_Sound;

	class APickupItem* CraftedItem;
	class APickupItem* SpawnCraftedItem(const FItemStruct* ItemToCraft);
	FTransform ItemToCraftOffsetTransform(const FVector & OffsetVector, const FRotator & OffsetRotator, const FVector& ItemInitialScale);

	// Crafting Sound
	int32 MiddleCraftSoundCounter;
	int32 CalculateHowManyMiddleCraftSoundHaveToPlay(const float & TimeToCraftAnItem);
	void StartPlayingCraftSound(const float & TimeToCraftAnItem);
	void PlayMiddleCraftSound();

	// Moving an item after it has been created
	bool bMoveCraftedItemToFinalPosition;
	bool bShouldScaleCraftedItem;
	FVector FinalLocationItem;
	FVector TargetScaleOfCraftedItem;
	void ItemWasMoved();
	void MoveCraftedItemToFinalPosition(float Delta);

	class AAlbertosPawn* AlbertosPawn;
		
};