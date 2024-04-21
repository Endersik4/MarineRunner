// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"

/// <summary>
/// A Component that will storage weapons in order that was taken.
/// Max amount of items can be customizable
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	bool CanPlayerTakeWeaponToInventory() const;
	FORCEINLINE int32 GetCurrentAmountOfWeapons() const { return WeaponsStorage.Num(); }
	FORCEINLINE void SetWeaponFromInventory(class IWeaponInterface* NewWeapon) { WeaponFromSlot = NewWeapon; }

	void AddNewWeaponToStorage(class IWeaponInterface* NewGun);

	//Removing weapon from Storage and sorting the rest of weapons
	void RemoveWeaponFromStorage(class IWeaponInterface* EquipedGun);

	//Get Weapon from Storage. If There is no KeyForWeapon as key in WeaponStorage then return CurrentWeapon equiped by Player. If found then Put away old weapon
	//@return true if new gun was found, false when player tried draw the same weapon
	bool GetWeaponFromStorage(int32 KeyForWeapon, class IWeaponInterface* CurrentWeapon);

	class IWeaponInterface* GetCurrentWeaponToDraw();
	int32 GetLastWeaponSlotFromStorage(class IWeaponInterface* ValueToIgnore);

	// Copy Current Equiped Weapons to Weapons To Load To Inventory, its for saving 
	void WeaponStorageToInitialWeaponInventory();
	void StartTimerForSpawnNewWeapons();

	// int32 - MAgazine Capacity, FString - Path to Class to spawn
	// Weapons Inventory to Load to the WeaponsStorage
	UPROPERTY(EditAnywhere, Category = "Weapon Inventory Settings Settings")
		TMap<int32, FString> WeaponsToLoadToInventory;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Inventory Settings Settings")
		int32 MaxAmountOfWeapons = 2;

	//UPROPERTY(Transient)
	// inventory for current equiped weapons 
		TMap < int32, class IWeaponInterface* > WeaponsStorage;
	//UPROPERTY(Transient)
		class IWeaponInterface* WeaponFromSlot = nullptr;

	void LoadWeapons();

	// Sort Weapons Storage in ascending order
	void SortWeapons();
};
