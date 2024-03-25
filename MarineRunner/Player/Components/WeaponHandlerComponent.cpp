// Copyright Adam Bartela.All Rights Reserved

#include "WeaponHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Gun/Gun.h"
#include "MarineRunner/Gun/Mods/ScopeGunMod.h"
#include "MarineRunner/Gun/Components/GunControlsComponent.h"
#include "MarineRunner/Gun/Components/GunReloadComponent.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Weapon Handler Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}
	
	FTimerHandle UpdateMouseSensitivitesHandle;
	GetWorld()->GetTimerManager().SetTimer(UpdateMouseSensitivitesHandle, this, &UWeaponHandlerComponent::LoadSavedSettingsFromGameInstance, 0.05f, false);
}

#pragma region ////////////////////////////////// GUN //////////////////////////////////
void UWeaponHandlerComponent::Shoot()
{
	if (!IsValid(Gun))
		return;

	Gun->SetShootButtonPressed(true);
	Gun->Shoot();
}

void UWeaponHandlerComponent::ReleasedShoot()
{
	if (!IsValid(Gun))
		return;

	Gun->ShootReleased();
}

void UWeaponHandlerComponent::Reload()
{
	if (!IsValid(Gun))
		return;

	Gun->GetGunReloadComponent()->PrepareToReload();
}

void UWeaponHandlerComponent::ADSPressed()
{
	if (!IsValid(Gun)) 
		return;

	if (Gun->GetGunReloadComponent()->GetIsReloading() && Gun->GetGunReloadComponent()->GetReloadOneBullet())
		Gun->GetGunReloadComponent()->CancelReload();

	if (!Gun->GetCanShoot()) 
		return;

	Player->MakeCrosshire(true);
	Player->SetMovementForceDividerWhenInADS(MovementForceDividerWhenInADS);

	if (IsValid(ADSInSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), ADSInSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("ADS In Sound is nullptr in Weapon Handler Component!"));

	bIsPlayerADS = true;
	Gun->AimTheGun(EStatusOfAimedGun::ESAG_ADS);

	if (CurrentScopeIndex >= MouseSensitivityWhenScope.Num())
		return;

	Player->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}

void UWeaponHandlerComponent::ADSReleased()
{
	if (!IsValid(Gun)|| !bIsPlayerADS) 
		return;

	Player->MakeCrosshire();
	Player->SetMovementForceDividerWhenInADS(1.f);

	if (IsValid(ADSOutSound)) 
		UGameplayStatics::SpawnSound2D(GetWorld(), ADSOutSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("ADS Out Sound is nullptr in Weapon Handler Component!"));

	bIsPlayerADS = false;

	Gun->AimTheGun(EStatusOfAimedGun::ESAG_HipFire);

	Player->ChangeMouseSensitivity(FSettingSavedInJsonFile(), true);

	if (Gun->GetUseScope())
		CurrentScopeIndex = Gun->GetScopeActor()->Zoom(0.f, true);
	else
		CurrentScopeIndex = 0;
}

void UWeaponHandlerComponent::UpdateWeaponInformationOnHud()
{
	if (!IsValid(Gun))
		return;
	
	Gun->GetGunControlsComponent()->UpdateWeaponDataInHud(true);
}

void UWeaponHandlerComponent::Zoom(float WheelAxis)
{
	if (!IsValid(Gun) || !bIsPlayerADS || WheelAxis == 0.f) 
		return;

	if (!Gun->GetUseScope())
		return;

	if (!IsValid(Gun->GetScopeActor()))
		return;

	CurrentScopeIndex = Gun->GetScopeActor()->Zoom(WheelAxis);

	if (CurrentScopeIndex >= MouseSensitivityWhenScope.Num())
		return;

	Player->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}
#pragma endregion 

#pragma region ////////////////////// EQUIP WEAPON FROM INVENTORY //////////////////////
void UWeaponHandlerComponent::SelectWeaponFromQuickInventory(int32 HandNumber)
{
	if (!bCanChangeWeapon|| bIsPlayerADS)
		return;

	const bool bDrawGunAccordingToHandNumber = Player->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, Gun);

	if (!bDrawGunAccordingToHandNumber)
		return;

	bCanChangeWeapon = false;
}

void UWeaponHandlerComponent::DrawNewGun()
{
	if (!IsValid(Player->GetWeaponInventoryComponent()->GetCurrentGunToDraw()))
		return;

	Gun = Player->GetWeaponInventoryComponent()->GetCurrentGunToDraw();
	Gun->GetGunControlsComponent()->DrawGun();
}

void UWeaponHandlerComponent::DropGun()
{
	if (!bCanChangeWeapon)
		return;

	if (!IsValid(Gun))
		return;

	Gun->GetGunControlsComponent()->SetDropGun(true);
	if (Player->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1)
		Gun->GetGunControlsComponent()->PutAwayGun();
	else 
		SelectWeaponFromQuickInventory(Player->GetWeaponInventoryComponent()->GetLastWeaponSlotFromStorage(Gun));
}

void UWeaponHandlerComponent::HideCurrentHoldingGun()
{
	if (!IsValid(Gun))
		return;

	Gun->GetGunControlsComponent()->HideGun();
}
#pragma endregion 

void UWeaponHandlerComponent::LoadSavedSettingsFromGameInstance()
{
	if (!IsValid(Player))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	TObjectPtr<AMarinePlayerController> MarinePlayerController = Cast<AMarinePlayerController>(Player->GetController());

	if (!IsValid(GameInstance) || !IsValid(MarinePlayerController))
		return;

	GameInstance->FindSavedValueAccordingToName(Player->GetMouseSensitivityJSON().FieldName, Player->GetMouseSensitivityJSON().FieldValue);
	const FSettingSavedInJsonFile& CurrentMouseSensName = MarinePlayerController->GetMouseSensitivity();

	if (CurrentMouseSensName == Player->GetMouseSensitivityJSON()) 
		Player->ChangeMouseSensitivity(Player->GetMouseSensitivityJSON(), true);

	for (FSettingSavedInJsonFile& CurrSetting : MouseSensitivityWhenScope)
	{
		GameInstance->FindSavedValueAccordingToName(CurrSetting.FieldName, CurrSetting.FieldValue);
		if (CurrentMouseSensName == CurrSetting) 
			Player->ChangeMouseSensitivity(CurrSetting);
	}
}