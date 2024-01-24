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

	int32 GetWeaponsStorageAmount() const { return WeaponsStorage.Num(); }
	int32 GetMaxAmount() const { return MaxAmountOfItems; }

	void SetWeaponsStorage(TMap < int32, class AGun* > NewWeaponsStorage) { WeaponsStorage = NewWeaponsStorage; }

	void AddNewWeaponToStorage(class AGun* NewGun);

	//Removing weapon from Storage and sorting the rest of weapons
	void RemoveWeaponFromStorage(class AGun* EquipedGun);

	//Get Weapon from Storage. If There is no KeyForWeapon as key in WeaponStorage then return CurrentWeapon equiped by Player
	//@return true if new gun was found, false when player tried draw the same weapon
	bool GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon);

	AGun* GetCurrentGunToDraw();

	TMap<int32, class AGun*> ReturnAllWeapons() { return WeaponsStorage; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
		int32 MaxAmountOfItems = 2;

	TMap < int32, class AGun* > WeaponsStorage;
	class AGun* GunFromInventory;
		
	void SortWeapons();

	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();
};
