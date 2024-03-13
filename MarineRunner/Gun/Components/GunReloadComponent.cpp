// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Gun/Components/GunReloadComponent.h"

#include "MarineRunner/Gun/Gun.h"
#include "MarineRunner/Gun/Components/GunControlsComponent.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Inventory/InventoryComponent.h"

UGunReloadComponent::UGunReloadComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UGunReloadComponent::BeginPlay()
{
	Super::BeginPlay();

	OriginalMagazineCapacity = MagazineCapacity;
	Gun = Cast<AGun>(GetOwner());
}

#pragma region ////////////////////////////////// RELOAD //////////////////////////////////////
bool UGunReloadComponent::CanReload()
{
	if (bIsReloading || GetWorld()->GetTimerManager().IsTimerActive(ReloadHandle))
		return false;

	if (!Gun->GetGunControlsComponent()->GetAmmunitionFromInventory() || MagazineCapacity == OriginalMagazineCapacity)
		return false;
	if (Gun->GetGunControlsComponent()->GetAmmunitionFromInventory()->Item_Amount <= 0)
		return false;

	if (!Gun->GetCanShoot())
		return false;

	return true;
}

void UGunReloadComponent::PrepareToReload()
{
	if (!CanReload())
		return;

	bIsReloading = true;

	Gun->SetCanShoot(false);
	Gun->ShootReleased();

	if (bReloadOneBullet)
		CurrentReloadType = GetCurrentReloadTypeAccordingToSituation();

	Gun->GetPlayer()->GetWeaponHandlerComponent()->ADSReleased();

	const FWeaponAnimation& ReloadAnimToPlay = ReloadAnimAccordingToSituation();
	Gun->PlayGivenWeaponWithArmsAnimation(ReloadAnimAccordingToSituation());

	GetWorld()->GetTimerManager().SetTimer(ReloadHandle, this, &UGunReloadComponent::Reload, ReloadAnimToPlay.ArmsActionAnim->GetPlayLength(), false);
}

void UGunReloadComponent::Reload()
{
	FItemStruct* AmmoFromInventory = Gun->GetGunControlsComponent()->GetAmmunitionFromInventory();
	if (!AmmoFromInventory)
		return;

	ReloadMagazine(AmmoFromInventory);
	if (AmmoFromInventory->Item_Amount <= 0)
		Gun->GetPlayer()->GetInventoryComponent()->DeleteItemFromInventory(*AmmoFromInventory);

	Gun->SetCanShoot(true);
	Gun->GetGunControlsComponent()->UpdateWeaponDataInHud(true);

	Gun->GetPlayer()->UpdateAlbertosInventory();

	GetWorld()->GetTimerManager().ClearTimer(ReloadHandle);
	bIsReloading = false;

	if (bReloadOneBullet)
		PrepareToReload();
}

void UGunReloadComponent::CancelReload()
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadHandle);

	Gun->SetCanShoot(true);
	bIsReloading = false;
	CurrentReloadType = ERT_EndReload;
}

const FWeaponAnimation& UGunReloadComponent::ReloadAnimAccordingToSituation()
{
	if (MagazineCapacity == 0)
		return WeaponReloadWithNoBulletsAnim;

	if (bReloadOneBullet)
	{
		if (CurrentReloadType == ERT_BeginEndReload)
			return WeaponReload_BeginEnd;

		if (CurrentReloadType == ERT_BeginReload)
			return WeaponReload_Begin;

		if (CurrentReloadType == ERT_EndReload)
			return WeaponReload_End;
	}

	return WeaponReloadAnim;
}

void UGunReloadComponent::ReloadMagazine(FItemStruct* AmmoFromInventory)
{
	if (bReloadOneBullet == true)
	{
		MagazineCapacity++;
		AmmoFromInventory->Item_Amount--;
		return;
	}

	int32 RestAmmo = OriginalMagazineCapacity - MagazineCapacity;
	if (AmmoFromInventory->Item_Amount < RestAmmo)
	{
		MagazineCapacity += AmmoFromInventory->Item_Amount;
		AmmoFromInventory->Item_Amount = 0;
	}
	else
	{
		AmmoFromInventory->Item_Amount -= RestAmmo;
		MagazineCapacity = OriginalMagazineCapacity;
	}
}

EReloadType UGunReloadComponent::GetCurrentReloadTypeAccordingToSituation()
{
	if (CurrentReloadType == ERT_EndReload && (MagazineCapacity == OriginalMagazineCapacity - 1 || Gun->GetGunControlsComponent()->GetAmmunitionFromInventory()->Item_Amount <= 1))
	{
		return ERT_BeginEndReload;
	}
	if (CurrentReloadType == ERT_EndReload || CurrentReloadType == ERT_BeginEndReload)
	{
		return ERT_BeginReload;
	}
	if ((CurrentReloadType == ERT_Reload || CurrentReloadType == ERT_BeginReload) && (MagazineCapacity == OriginalMagazineCapacity - 1 || Gun->GetGunControlsComponent()->GetAmmunitionFromInventory()->Item_Amount <= 1))
	{
		return ERT_EndReload;
	}

	return ERT_Reload;
}

bool UGunReloadComponent::CanShootWhileReloading()
{
	if (bReloadOneBullet && bIsReloading && MagazineCapacity > 0)
	{
		CancelReload();
		return true;
	}
	else if (bIsReloading)
		return false;

	return true;
}
#pragma endregion