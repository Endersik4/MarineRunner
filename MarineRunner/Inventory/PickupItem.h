// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/TakeInterface.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "PickupItem.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetDissolveMaterial(class AMarineCharacter* Player, float TimeToEndDisolve, UMaterialInstance* OverlayInstanceMaterial);

	FORCEINLINE const FName GetItemRowName() const { return ItemRowName; }
	FORCEINLINE UStaticMeshComponent* GetItemMesh() { return ItemMesh; }

	FORCEINLINE void SetItemAmountMultiplier(int32 NewAmountMultiplier) { AmountMultiplier = NewAmountMultiplier; }
	FORCEINLINE void SetItemWasOnceTaken(bool bNewWasOnceTaken) { bWasOnceTaken = bNewWasOnceTaken; }
	FORCEINLINE void SetCurrentMagazineCapacity(int32 NewCurrentMagazineCapacity) { CurrentMagazineCapacity = NewCurrentMagazineCapacity; }

	void SaveItemIfSpawnedRunTime();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USoundOnHitComponent* SoundOnHitComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		FName ItemRowName;
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Sounds")
		USoundBase* PickUpSound;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
		FName DisolveScalarParameterName = FName(TEXT("Dissolve"));
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
		float DissolveStartValue = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings|Craft")
		float DissolveEndValue = -0.4f;

	// Take
	bool AddAmountToItemIfFound(FItemStruct* ItemFromInventory, float AmountToAdd);
	void AddCraftRecipeIfCraftable(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable);
	void SpawnWeaponForPlayer(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable);

	int32 AmountMultiplier = 1;

	bool bWasOnceTaken;
	int32 CurrentMagazineCapacity;

	//Dissolve Material
	bool bShouldDissolve;
	float DissolveTimeElapsed;
	float TimeToCraftAnItem = 4.f;
	UMaterialInstanceDynamic* DissolveDynamicMaterial;
	void Dissolve(float Delta);

	void SaveItemWasTaken();
	void SaveItem(class ASavedDataObject* SavedDataObject,const FCustomDataSaved& DataToSave);
	void DisableItem();

	int32 CurrentUniqueID = 0;
};
