// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"

// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponsStorage.Empty();
}

void UWeaponInventoryComponent::AddNewWeaponToStorage(AGun* NewGun)
{
	if (!NewGun) return;

	int32 NumberForWeapon = WeaponsStorage.Num() + 1;
	WeaponsStorage.Add(NumberForWeapon, NewGun);
}

void UWeaponInventoryComponent::RemoveWeaponFromStorage(AGun* EquipedGun)
{
	int32 KeyForEquipedGun = *WeaponsStorage.FindKey(EquipedGun);
	WeaponsStorage.Remove(KeyForEquipedGun);

	SortWeapons();
}

AGun* UWeaponInventoryComponent::GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon)
{
	if (WeaponsStorage.Find(KeyForWeapon) == nullptr) return CurrentWeapon;
	AGun* Gun = *WeaponsStorage.Find(KeyForWeapon);
	if (Gun == CurrentWeapon) return CurrentWeapon;

	if (CurrentWeapon)
	{
		CurrentWeapon->ShootReleased();
		//CurrentWeapon->SetGunSwayWhileMovingTimer(true);
		if (CurrentWeapon->GetIsReloading()) CurrentWeapon->CancelReload();
		CurrentWeapon->SetActorHiddenInGame(true);
	}

	//Gun->SetGunSwayWhileMovingTimer();
	Gun->UpdateWeaponDataInHud(true, true);
	Gun->SetActorHiddenInGame(false);

	return Gun;
}

void UWeaponInventoryComponent::SortWeapons()
{
	TArray<AGun*> Guns;
	WeaponsStorage.GenerateValueArray(Guns);
	WeaponsStorage.Empty();
	for (int32 i = 1; i <= Guns.Num(); i++)
	{
		WeaponsStorage.Add(i, Guns[i - 1]);
	}
}

void UWeaponInventoryComponent::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}

