// Copyright Adam Bartela.All Rights Reserved

#include "WeaponInventoryComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Gun/Gun.h"
#include "MarineRunner/Gun/Components/GunControlsComponent.h"
#include "MarineRunner/Gun/Components/GunReloadComponent.h"

// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWeaponInventoryComponent::StartTimerForSpawnNewWeapons()
{
	FTimerHandle SpawnWeaponsFromInventoryHandle;
	GetWorld()->GetTimerManager().SetTimer(SpawnWeaponsFromInventoryHandle, this, &UWeaponInventoryComponent::SpawnWeaponsFromInventory, 0.02f, false);
}

void UWeaponInventoryComponent::SpawnWeaponsFromInventory()
{
	TObjectPtr<AMarineCharacter> MarinePawn = Cast<AMarineCharacter>(GetOwner());

	if (!IsValid(MarinePawn) || InitialWeaponInventory.Num() == 0)
		return;

	for (const TPair<int32, FString> CurrentPair : InitialWeaponInventory)
	{
		const FSoftClassPath GunClassPath = CurrentPair.Value;
		if (!GunClassPath.TryLoadClass<UObject>())
			continue;

		TObjectPtr<AGun> SpawnedGun = GetWorld()->SpawnActor<AGun>(GunClassPath.TryLoadClass<UObject>(), FTransform(FRotator(0.f), FVector(0.f), FVector(1.f)));
		if (!IsValid(SpawnedGun))
			continue;

		SpawnedGun->AttachToComponent(MarinePawn->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SpawnedGun->GetGunControlsComponent()->GetAttachToSocketName());
		SpawnedGun->GetGunControlsComponent()->TakeGun(MarinePawn, true, CurrentPair.Key);
	}
}

void UWeaponInventoryComponent::AddNewWeaponToStorage(TObjectPtr<AGun> NewGun)
{
	if (!IsValid(NewGun)) 
		return;

	int32 WeaponNumber = WeaponsStorage.Num() + 1;
	WeaponsStorage.Add(WeaponNumber, NewGun);
}

void UWeaponInventoryComponent::RemoveWeaponFromStorage(TObjectPtr<AGun> EquipedGun)
{
	int32 KeyForEquipedGun = *WeaponsStorage.FindKey(EquipedGun);
	WeaponsStorage.Remove(KeyForEquipedGun);
	
	SortWeapons();
}

bool UWeaponInventoryComponent::GetWeaponFromStorage(int32 KeyForWeapon, TObjectPtr<AGun> CurrentWeapon)
{
	if (!IsValid(CurrentWeapon))
		return false;

	if (!WeaponsStorage.Find(KeyForWeapon)) 
		return false;
	GunFromInventory = *WeaponsStorage.Find(KeyForWeapon);
	if (GunFromInventory == CurrentWeapon)
		return false;

	CurrentWeapon->GetGunControlsComponent()->PutAwayGun();

	return true;
}

TObjectPtr<AGun> UWeaponInventoryComponent::GetCurrentGunToDraw()
{
	return GunFromInventory;
}

int32 UWeaponInventoryComponent::GetLastWeaponSlotFromStorage(TObjectPtr<AGun> ValueToIgnore)
{
	TArray<int32> SlotsForGunGun;
	WeaponsStorage.GenerateKeyArray(SlotsForGunGun);
	for (int32 i = SlotsForGunGun.Num() - 1; i >= 0; i--)
	{
		if (!WeaponsStorage.Find(SlotsForGunGun[i]))
			continue;

		if (*WeaponsStorage.Find(SlotsForGunGun[i]) != ValueToIgnore)
			return SlotsForGunGun[i];
	}
	return 0;
}

void UWeaponInventoryComponent::SortWeapons()
{
	TArray<TObjectPtr<AGun>> Guns;
	WeaponsStorage.GenerateValueArray(Guns);
	WeaponsStorage.Empty();
	for (int32 i = 1; i <= Guns.Num(); i++)
	{
		WeaponsStorage.Add(i, Guns[i-1]);
	}
}

void UWeaponInventoryComponent::WeaponStorageToInitialWeaponInventory()
{
	InitialWeaponInventory.Empty();
	for (const TPair<int32, TObjectPtr<class AGun> > CurrentPair : WeaponsStorage)
	{
		InitialWeaponInventory.Add(CurrentPair.Value->GetGunReloadComponent()->GetMagazineCapacity(), CurrentPair.Value->GetClass()->GetClassPathName().ToString());
	}
}

bool UWeaponInventoryComponent::CanPlayerTakeWeaponToInventory() const
{
	return WeaponsStorage.Num() < MaxAmountOfItems;
}
