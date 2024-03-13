// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Gun/Components/GunControlsComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Gun/Gun.h"
#include "MarineRunner/Gun/Components/GunReloadComponent.h"
#include "MarineRunner/Objects/Interactive/PickableItem.h"
#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"

UGunControlsComponent::UGunControlsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGunControlsComponent::BeginPlay()
{
	Super::BeginPlay();

	Gun = Cast<AGun>(GetOwner());
}

#pragma region ////////////////////////////////// TAKE ////////////////////////////////////////
void UGunControlsComponent::TakeGun(TObjectPtr<AMarineCharacter> NewPlayer, bool bWasTaken, int32 SavedMagazineCapacity)
{
	if (!IsValid(NewPlayer) || !IsValid(Gun))
		return;

	Gun->SetPlayer(NewPlayer);
	Gun->SetCanShoot(false); //cant shoot until Weapon First Take Anim is finished

	Player = NewPlayer;
	Player->GetArmsSkeletalMesh()->SetForceRefPose(false);
	Player->GetWeaponInventoryComponent()->SetGunFromInventory(nullptr);
	Player->GetWeaponInventoryComponent()->AddNewWeaponToStorage(Gun);
	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);
	Player->GetWeaponHandlerComponent()->HideCurrentHoldingGun();
	Player->GetWeaponHandlerComponent()->SetGun(Gun);
	Player->GetHudWidget()->ShowWeaponOnHud();

	Gun->PlayGivenWeaponWithArmsAnimation(bWasTaken ? WeaponDrawAnim : WeaponFirstTimeTakeAnim);

	if (bWasTaken)
		Gun->GetGunReloadComponent()->SetMagazineCapacity(SavedMagazineCapacity);
	else
		AddAmmoToInventory();

	UpdateWeaponDataInHud(true, true);

	FTimerHandle TakeGunHandle;
	GetWorld()->GetTimerManager().SetTimer(TakeGunHandle, Gun.Get(), &AGun::SetCanShootAgain, (bWasTaken ? WeaponDrawAnim : WeaponFirstTimeTakeAnim).ArmsActionAnim->GetPlayLength(), false);
}

void UGunControlsComponent::DrawGun()
{
	if (!IsValid(Player) || !IsValid(Gun))
		return;

	Gun->GetGunReloadComponent()->CancelReload();
	Gun->SetActorHiddenInGame(false);
	Gun->SetActorTickEnabled(true);
	Gun->SetCanShoot(false);
	Gun->PlayGivenWeaponWithArmsAnimation(WeaponDrawAnim);

	Player->GetWeaponHandlerComponent()->SetGun(Gun);
	Player->GetHudWidget()->ShowWeaponOnHud();

	if (IsValid(DrawGunSound))
		UGameplayStatics::PlaySound2D(GetWorld(), DrawGunSound);

	UpdateWeaponDataInHud(true, true);

	FTimerHandle DrawGunHandle;
	GetWorld()->GetTimerManager().SetTimer(DrawGunHandle, Gun.Get(), &AGun::SetCanShootAgain, WeaponDrawAnim.ArmsActionAnim->GetPlayLength(), false);
}

void UGunControlsComponent::PutAwayGun()
{
	if (!IsValid(Player) || !IsValid(Gun))
		return;

	Gun->GetGunReloadComponent()->CancelReload();
	Gun->SetCanShoot(false);
	Gun->PlayGivenWeaponWithArmsAnimation(WeaponPutAwayAnim);
	Gun->ShootReleased();
	Gun->CancelShoot();

	Player->GetWeaponHandlerComponent()->SetGun(nullptr);
	Player->GetHudWidget()->ShowWeaponOnHud(false);

	if (IsValid(PutAwayGunSound))
		UGameplayStatics::PlaySound2D(GetWorld(), PutAwayGunSound);

	FTimerHandle HideGunHandle;
	GetWorld()->GetTimerManager().SetTimer(HideGunHandle, this, &UGunControlsComponent::HideGun, WeaponPutAwayAnim.ArmsActionAnim->GetPlayLength(), false);
}

void UGunControlsComponent::HideGun()
{
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorTickEnabled(false);
	Gun->GetGunSkeletalMesh()->Stop();

	Player->GetArmsSkeletalMesh()->Stop();

	// Back to T-Pose if there is no gun to draw after dropping current gun
	if (Player->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1 && bDropGun)
	{
		Player->GetArmsSkeletalMesh()->SetForceRefPose(true);
	}
	else
		Player->GetWeaponHandlerComponent()->DrawNewGun();

	DropGun();
}
#pragma endregion

#pragma region ////////////////////////////////// DROP ////////////////////////////////////////
void UGunControlsComponent::DropGun()
{
	if (!bDropGun)
		return;

	if (!IsValid(ItemToSpawnAfterDropGun))
		return;

	Player->GetWeaponInventoryComponent()->RemoveWeaponFromStorage(Gun);

	if (IsValid(DropGunSound))
		UGameplayStatics::PlaySound2D(GetWorld(), DropGunSound);

	FVector LocationToSpawnItemGun = Player->GetCameraLocation() + Player->GetCamera()->GetForwardVector() * DistanceToDropGun;
	FTransform ItemGunTransform = FTransform(FRotator(0.f), LocationToSpawnItemGun);
	TObjectPtr<APickupItem> SpawnedGunItem = GetWorld()->SpawnActorDeferred<APickupItem>(ItemToSpawnAfterDropGun, ItemGunTransform);
	if (IsValid(SpawnedGunItem))
	{
		SpawnedGunItem->SetCurrentMagazineCapacity(Gun->GetGunReloadComponent()->GetMagazineCapacity());
		SpawnedGunItem->SetItemWasOnceTaken(true);
		SpawnedGunItem->SaveItemIfSpawnedRunTime();
		SpawnedGunItem->FinishSpawning(ItemGunTransform);
	}

	FItemStruct* WeaponInformation = Player->GetInventoryComponent()->GetItemInformationFromDataTable(SpawnedGunItem->GetItemRowName());
	if (WeaponInformation)
		Player->GetInventoryComponent()->Inventory_Items.Remove(*WeaponInformation);

	Player->UpdateAlbertosInventory(true);
	Gun->Destroy();
}
#pragma endregion

#pragma region /////////////////////////////// INVENTORY //////////////////////////////////////
void UGunControlsComponent::AddAmmoToInventory()
{
	if (!IsValid(Player))
		return;

	FItemStruct* AmmunitionFromInventory = Player->GetInventoryComponent()->GetItemFromInventory(RowNameForAmmunitionItem);

	if (AmmunitionFromInventory) // if ammo found in inventory then add the amount
	{
		AmmunitionFromInventory->Item_Amount += StoredAmmo;
	}
	else
		Player->GetInventoryComponent()->AddNewItemToInventory(RowNameForAmmunitionItem, StoredAmmo);
}

#pragma endregion

#pragma region ///////////////////////////////// HUD //////////////////////////////////////////
void UGunControlsComponent::UpdateWeaponDataInHud(bool bUpdateStoredAmmoText, bool bUpdateWeaponImage)
{
	if (!IsValid(Player))
		return;
	if (!IsValid(Player->GetHudWidget()))
		return;

	Player->GetHudWidget()->UpdateAmmoInMagazineAmountText(Gun->GetGunReloadComponent()->GetMagazineCapacity());

	if (bUpdateStoredAmmoText)
	{
		FItemStruct* AmmoFromInventory = Player->GetInventoryComponent()->GetItemFromInventory(RowNameForAmmunitionItem);
		Player->GetHudWidget()->UpdateStoredAmmoAmountText(AmmoFromInventory ? AmmoFromInventory->Item_Amount : 0);
	}
	if (bUpdateWeaponImage)
	{
		Player->GetHudWidget()->UpdateWeaponImage(GunHUDTexture, bAmmoCounterBelowGunHUD);
	}
}
#pragma endregion

FItemStruct* UGunControlsComponent::GetAmmunitionFromInventory() const
{
	return Player->GetInventoryComponent()->GetItemFromInventory(RowNameForAmmunitionItem);
}