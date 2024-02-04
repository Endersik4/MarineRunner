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

	virtual void LoadData(int32 StateOfData = 0) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ChangeSimulatingPhysics(bool bChange = true);

	virtual void SetDissolveMaterial(class AMarineCharacter* Player, UMaterialInstance* NewMaterial, USkeletalMeshComponent* SkeletalMesh = nullptr);

	virtual void SetCollisionNewResponse(ECollisionChannel ChannelName, ECollisionResponse NewResponse);

	FORCEINLINE const FName GetItemRowName() const { return ItemRowName; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USoundOnHitComponent* SoundOnHitComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PickUpSound;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		FName ItemRowName;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		float DissolveStartValue = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		float DissolveEndValue = -0.4f;

	bool AddAmountToItemIfFound(FItemStruct* ItemFromInventory, float AmountToAdd);
	void AddCraftRecipeIfCraftable(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable);
	void SpawnWeaponForPlayer(class AMarineCharacter* Player, FItemStruct* ItemDataFromDataTable);

	//Dissolve Material
	bool bShouldDissolve;
	float DissolveTimeElapsed;
	float TimeToCraftAnItem = 4.f;
	UMaterialInstanceDynamic* DissolveDynamicMaterial;
	USkeletalMeshComponent* WeaponSkeletalMesh;
	void Dissolve(float Delta);

	void SaveItemWasTaken();


};
