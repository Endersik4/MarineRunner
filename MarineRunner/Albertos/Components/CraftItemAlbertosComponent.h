// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"

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

	void CraftPressed(TObjectPtr<class AMarineCharacter> Player, const FItemStruct* ItemToCraft, int32 ItemAmountMultiplier);
	void CraftingFinished();

	FORCEINLINE const bool IsCraftedItemValid() const;
private:

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		FName ItemSpawnLocationSocketName = TEXT("ItemSpawnLocation");
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		TObjectPtr<UMaterialInstance> OverlayCraftingMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds")
		TObjectPtr<USoundBase> Craft_Start_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds")
		TObjectPtr<USoundBase> Craft_Middle_Sound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds")
		TObjectPtr<USoundBase> Craft_End_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Move Item")
		float ItemMoveSpeedAfterCrafting = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Move Item")
		float ItemScaleSpeedAfterCrafting = 11.f;
	UPROPERTY(EditDefaultsOnly, Category = "Move Item")
		FName FinalItemLocationSocketName = TEXT("FinalItemPosition");
	UPROPERTY(EditDefaultsOnly, Category = "Move Item")
		float FinalLocationItemTolerance = 10.f;

	UPROPERTY(Transient)
		TObjectPtr<class APickupItem> CraftedItem = nullptr;

	TObjectPtr<APickupItem> SpawnCraftedItem(const FItemStruct* ItemToCraft);
	FTransform ItemToCraftOffsetTransform(const FVector& OffsetVector, const FRotator& OffsetRotator, const FVector& ItemInitialScale);

	UPROPERTY(Transient)
		int32 MiddleCraftSoundCounter;
	int32 CalculateHowManyMiddleCraftSoundHaveToPlay(const float& TimeToCraftAnItem);
	void StartPlayingCraftSound(const float& TimeToCraftAnItem);
	void PlayMiddleCraftSound();

	// Moving an item after it has been created
	UPROPERTY(Transient)
		bool bMoveCraftedItemToFinalPosition = false;
	UPROPERTY(Transient)
		bool bShouldScaleCraftedItem = false;
	UPROPERTY(Transient)
		FVector FinalCraftedItemLocation = FVector::Zero();
	UPROPERTY(Transient)
		FVector TargetScaleOfCraftedItem = FVector::Zero();

	void ItemWasMoved();
	void MoveCraftedItemToFinalPosition(float Delta);

	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosPawn = nullptr;
		
};
