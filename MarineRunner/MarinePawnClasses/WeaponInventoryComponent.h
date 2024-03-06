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
	FORCEINLINE void SetGunFromInventory(TObjectPtr<class AGun> NewGun) { GunFromInventory = NewGun; }

	void AddNewWeaponToStorage(TObjectPtr<class AGun> NewGun);

	//Removing weapon from Storage and sorting the rest of weapons
	void RemoveWeaponFromStorage(TObjectPtr<class AGun> EquipedGun);

	//Get Weapon from Storage. If There is no KeyForWeapon as key in WeaponStorage then return CurrentWeapon equiped by Player
	//@return true if new gun was found, false when player tried draw the same weapon
	bool GetWeaponFromStorage(int32 KeyForWeapon, TObjectPtr<class AGun> CurrentWeapon);

	TObjectPtr<AGun> GetCurrentGunToDraw();
	int32 GetLastWeaponSlotFromStorage(TObjectPtr<class AGun> ValueToIgnore);

	void SaveInitialWeaponInventory();
	void StartTimerForSpawnNewWeapons();

	// int32 - MAgazine Capacity, FString - Path to Class to spawn
	UPROPERTY(EditAnywhere, Category = "Weapon Inventory Settings Settings")
		TMap<int32, FString> InitialWeaponInventory;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Inventory Settings Settings")
		int32 MaxAmountOfItems = 2;

	TMap < int32, TObjectPtr<class AGun> > WeaponsStorage;
	UPROPERTY(Transient)
		TObjectPtr<class AGun> GunFromInventory;

	void SpawnWeaponsFromInventory();
	void SortWeapons();
};
