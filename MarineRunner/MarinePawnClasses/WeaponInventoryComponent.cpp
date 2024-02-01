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
	
	SetUpMarinePawn();
	FTimerHandle test;
	GetWorld()->GetTimerManager().SetTimer(test, this, &UWeaponInventoryComponent::SpawnWeaponsFromInventory, 0.1f, false);
}

void UWeaponInventoryComponent::SpawnWeaponsFromInventory()
{
	if (IsValid(MarinePawn) == false || InitialWeaponInventory.Num() == 0)
		return;

	TArray<TSubclassOf<AGun>> WeaponsToSpawn; 
	InitialWeaponInventory.GenerateValueArray(WeaponsToSpawn);

	for (TSubclassOf<AGun> CurrentWeaponToSpawn : WeaponsToSpawn)
	{
		AGun* SpawnedGun = GetWorld()->SpawnActorDeferred<AGun>(CurrentWeaponToSpawn, FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));
		if (IsValid(SpawnedGun) == false)
			continue;

		SpawnedGun->AttachToComponent(MarinePawn->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(TEXT("Weapon")));
		SpawnedGun->TakeGun(MarinePawn);
		SpawnedGun->FinishSpawning(FTransform(FRotator(0.f, 90.f, 0.f), FVector(0.f), FVector(1.f)));

		AddNewWeaponToStorage(SpawnedGun);
	}
}

void UWeaponInventoryComponent::AddNewWeaponToStorage(AGun* NewGun)
{
	if (IsValid(NewGun) == false) 
		return;

	int32 WeaponNumber = WeaponsStorage.Num() + 1;
	WeaponsStorage.Add(WeaponNumber, NewGun);
}

void UWeaponInventoryComponent::RemoveWeaponFromStorage(AGun* EquipedGun)
{
	int32 KeyForEquipedGun = *WeaponsStorage.FindKey(EquipedGun);
	WeaponsStorage.Remove(KeyForEquipedGun);

	SortWeapons();
}

bool UWeaponInventoryComponent::GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon)
{
	if (IsValid(CurrentWeapon) == false)
		return false;

	if (WeaponsStorage.Find(KeyForWeapon) == nullptr) 
		return false;
	GunFromInventory = *WeaponsStorage.Find(KeyForWeapon);
	if (GunFromInventory == CurrentWeapon)
		return false;

	CurrentWeapon->PutAwayGun();

	return true;
}

AGun* UWeaponInventoryComponent::GetCurrentGunToDraw()
{
	return GunFromInventory;
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

