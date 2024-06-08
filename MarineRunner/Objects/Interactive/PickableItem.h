// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/Interfaces/TakeInterface.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "PickableItem.generated.h"

UCLASS()
class MARINERUNNER_API APickupItem : public AActor, public ITakeInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void TakeItem(class AMarineCharacter* Player) override;
	virtual void ItemHover(class AMarineCharacter* Character) override;
	virtual void ItemUnHover(class AMarineCharacter* Character) override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetDissolveMaterial(class AMarineCharacter* Player, float TimeToEndDisolve, UMaterialInstance* OverlayInstanceMaterial);

	FORCEINLINE const FName GetItemRowName() const { return ItemRowName; }
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetItemMesh() { return ItemMesh; }

	FORCEINLINE void SetItemAmountMultiplier(int32 NewAmountMultiplier) { AmountMultiplier = NewAmountMultiplier; }
	FORCEINLINE void SetItemWasOnceTaken(bool bNewWasOnceTaken) { bWasOnceTaken = bNewWasOnceTaken; }
	FORCEINLINE void SetSavedValue(int32 NewCurrentMagazineCapacity) { CurrentMagazineCapacity = NewCurrentMagazineCapacity; }

	void SaveItemIfSpawnedRunTime();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> ItemMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USoundOnHitComponent> SoundOnHitComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
	FName ItemRowName = FName("null");
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Sounds")
	TObjectPtr<USoundBase> PickUpSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
	FName DisolveScalarParameterName = FName(TEXT("Dissolve"));
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
	float DissolveStartValue = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
	float DissolveEndValue = -0.4f;

	// Take
	bool AddAmountToItemIfFound(FItemStruct* ItemFromInventory, float AmountToAdd);
	void AddCraftRecipeIfCraftable(TObjectPtr<class AMarineCharacter> Player, FItemStruct* ItemDataFromDataTable);
	void SpawnWeaponForPlayer(TObjectPtr<class AMarineCharacter>, FItemStruct* ItemDataFromDataTable);

	UPROPERTY(Transient)
	int32 AmountMultiplier = 1;
	UPROPERTY(Transient)
	bool bWasOnceTaken = false;
	UPROPERTY(Transient)
	int32 CurrentMagazineCapacity = 0;

	//Dissolve Material
	UPROPERTY(Transient)
	bool bShouldDissolve = false;
	UPROPERTY(Transient)
	float DissolveTimeElapsed = 0.f;
	UPROPERTY(Transient)
	float TimeToCraftAnItem = 4.f;
	void Dissolve(float Delta);
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DissolveDynamicMaterial = nullptr;

	void SaveItemWasTaken();
	void SaveItem(TObjectPtr<class ASavedDataObject> SavedDataObject, const FCustomDataSaved& DataToSave);
	void DisableItem(bool bDisable = true);

	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;
};
