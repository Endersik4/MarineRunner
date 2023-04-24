// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/Widgets/HUDWidget.h"

// Sets default values for this component's properties
UWeaponInventoryComponent::UWeaponInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponInventoryComponent::AddNewWeaponToStorage(AGun* NewGun)
{
	int32 NumberForWeapon = WeaponsStorage.Num() + 1;
	WeaponsStorage.Add(NumberForWeapon, NewGun);
}

void UWeaponInventoryComponent::RemoveWeaponFromStorage(AGun* EquipedGun)
{
	int32 KeyForEquipedGun = *WeaponsStorage.FindKey(EquipedGun);
	WeaponsStorage.FindAndRemoveChecked(KeyForEquipedGun);
}

AGun* UWeaponInventoryComponent::GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon)
{
	AGun* Gun = *WeaponsStorage.Find(KeyForWeapon);
	if (Gun == nullptr) return CurrentWeapon;

	if (CurrentWeapon) CurrentWeapon->SetActorHiddenInGame(true);
	//Gun->SetHudWidget(MarinePawn->GetHudWidget());
	Gun->SetWeaponInHud(true, true);
	Gun->SetActorHiddenInGame(false);

	return Gun;
}

void UWeaponInventoryComponent::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(GetOwner());
}

