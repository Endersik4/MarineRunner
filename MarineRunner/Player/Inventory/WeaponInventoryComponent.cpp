// Copyright Adam Bartela.All Rights Reserved

#include "WeaponInventoryComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Weapon/WeaponBase.h"

UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	StartTimerForSpawnNewWeapons();
	
}

void UWeaponInventoryComponent::StartTimerForSpawnNewWeapons()
{
	FTimerHandle SpawnWeaponsFromInventoryHandle;
	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponsFromInventoryHandle, this, &UWeaponInventoryComponent::LoadWeapons, TimeToSpawnWeaponsFromInventory, false);
}

void UWeaponInventoryComponent::LoadWeapons()
{
	if (!IsValid(GetOwner()))
		return;

	TObjectPtr<AMarineCharacter> MarinePawn = Cast<AMarineCharacter>(GetOwner());

	if (!IsValid(MarinePawn) || WeaponsToLoadToInventory.Num() == 0)
		return;

	for (const TPair<int32, FSoftClassPath> CurrentPair : WeaponsToLoadToInventory)
	{
		if (!CurrentPair.Value.TryLoadClass<UObject>())
			continue;

		TObjectPtr<AWeaponBase> SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(CurrentPair.Value.TryLoadClass<UObject>(), FTransform(FRotator(0.f), FVector(0.f), FVector(1.f)));
		if (!SpawnedWeapon)
			continue;

		SpawnedWeapon->PickUpWeaponItem(MarinePawn, true, CurrentPair.Key);
	}
}

void UWeaponInventoryComponent::AddNewWeaponToStorage(TObjectPtr<AWeaponBase> NewGun)
{
	if (!IsValid(NewGun))
		return;

	const int32& WeaponNumber = WeaponsStorage.Num() + 1;
	WeaponsStorage.Add(WeaponNumber, NewGun);
}

void UWeaponInventoryComponent::RemoveWeaponFromStorage(TObjectPtr<AWeaponBase> EquipedGun)
{
	const int32& KeyForEquipedGun = *WeaponsStorage.FindKey(EquipedGun);
	WeaponsStorage.Remove(KeyForEquipedGun);
	
	SortWeapons();
}

bool UWeaponInventoryComponent::GetWeaponFromStorage(int32 KeyForWeapon, TObjectPtr<AWeaponBase> CurrentWeapon)
{
	if (!IsValid(CurrentWeapon))
		return false;

	if (!WeaponsStorage.Find(KeyForWeapon)) 
		return false;
	WeaponFromSlot = *WeaponsStorage.Find(KeyForWeapon);
	if (WeaponFromSlot == CurrentWeapon)
		return false;

	CurrentWeapon->PutAwayWeapon();
	return true;
}

TObjectPtr<AWeaponBase> UWeaponInventoryComponent::GetCurrentWeaponToDraw()
{
	return WeaponFromSlot;
}

int32 UWeaponInventoryComponent::GetLastWeaponSlotFromStorage(TObjectPtr<AWeaponBase> ValueToIgnore)
{
	TArray<int32> AllWeaponInventorySlots;
	WeaponsStorage.GenerateKeyArray(AllWeaponInventorySlots);
	for (int32 i = AllWeaponInventorySlots.Num() - 1; i >= 0; i--)
	{
		if (!WeaponsStorage.Find(AllWeaponInventorySlots[i]))
			continue;

		if (*WeaponsStorage.Find(AllWeaponInventorySlots[i]) != ValueToIgnore)
			return AllWeaponInventorySlots[i];
	}
	return 0;
}

void UWeaponInventoryComponent::SortWeapons()
{
	TArray<TObjectPtr<AWeaponBase> > Weapons;
	WeaponsStorage.GenerateValueArray(Weapons);
	WeaponsStorage.Empty();
	for (int32 i = 1; i <= Weapons.Num(); i++)
	{
		WeaponsStorage.Add(i, Weapons[i-1]);
	}
}

void UWeaponInventoryComponent::WeaponStorageToInitialWeaponInventory()
{
	WeaponsToLoadToInventory.Empty();
	for (const TPair<int32, TObjectPtr<AWeaponBase> > CurrentPair : WeaponsStorage)
	{
		WeaponsToLoadToInventory.Add(CurrentPair.Value->GetIntValueToSave(), CurrentPair.Value->GetPathToWeaponClass());
	}
}

bool UWeaponInventoryComponent::CanPlayerTakeWeaponToInventory() const
{
	return WeaponsStorage.Num() < MaxAmountOfWeapons;
}
