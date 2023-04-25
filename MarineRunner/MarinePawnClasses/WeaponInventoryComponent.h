// Fill out your copyright notice in the Description page of Project Settings.

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	int32 GetWeaponsStorageAmount() const { return WeaponsStorage.Num(); }
	int32 GetMaxAmount() const { return MaxAmountOfItems; }

	void AddNewWeaponToStorage(class AGun* NewGun);

	//Removing weapon from Storage and sorting the rest of weapons
	void RemoveWeaponFromStorage(class AGun* EquipedGun);

	//Get Weapon from Storage. If There is no KeyForWeapon as key in WeaponStorage then return CurrentWeapon equiped by Player
	//@return Gun from Storage at given Key
	class AGun* GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
		int32 MaxAmountOfItems = 2;

	TMap < int32, class AGun* > WeaponsStorage;
		
	void SortWeapons();

	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();
};
