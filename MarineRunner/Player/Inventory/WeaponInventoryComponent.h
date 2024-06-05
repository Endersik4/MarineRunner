// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"

class AWeaponBase;

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
	FORCEINLINE void SetWeaponFromInventory(TObjectPtr<AWeaponBase> NewWeapon) { WeaponFromSlot = NewWeapon; }

	void AddNewWeaponToStorage(TObjectPtr<AWeaponBase> NewGun);

	//Removing weapon from Storage and sorting the rest of weapons
	void RemoveWeaponFromStorage(TObjectPtr<AWeaponBase> EquipedGun);

	//Get Weapon from Storage. If There is no KeyForWeapon as key in WeaponStorage then return CurrentWeapon equiped by Player. If found then Put away old weapon
	//@return true if new gun was found, false when player tried draw the same weapon
	bool GetWeaponFromStorage(int32 KeyForWeapon, TObjectPtr<AWeaponBase> CurrentWeapon);

	TObjectPtr<AWeaponBase> GetCurrentWeaponToDraw();
	int32 GetLastWeaponSlotFromStorage(TObjectPtr<AWeaponBase> ValueToIgnore);

	// Copy Current Equiped Weapons to Weapons To Load To Inventory, its for saving 
	void WeaponStorageToInitialWeaponInventory();
	void StartTimerForSpawnNewWeapons();

	// int32 - MAgazine Capacity, FSoftClassPath - Path to Class to spawn
	// Weapons Inventory to Load to the WeaponsStorage
	UPROPERTY(EditAnywhere, Category = "Weapon Inventory Settings Settings")
		TMap<int32, FSoftClassPath> WeaponsToLoadToInventory;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Inventory Settings Settings")
		int32 MaxAmountOfWeapons = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Inventory Settings Settings")
		float TimeToSpawnWeaponsFromInventory = 0.02f;

	// inventory for current equiped weapons 
	UPROPERTY(Transient)
		TMap < int32, TObjectPtr<AWeaponBase> > WeaponsStorage;
	// Current equiped weapon
	UPROPERTY(Transient)
		TObjectPtr<AWeaponBase> WeaponFromSlot = nullptr;

	void LoadWeapons();

	// When a weapon is removed from the inventory, the SortWeapons() function corrects the order of the weapons, e.g. the weapon from slot 1 was removed, and then the weapon from slot 2 was moved to slot 1.
	void SortWeapons();
};
