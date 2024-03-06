// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"
#include "MarineRunner/GunClasses/Gun.h"
#include "MarineRunner/GunClasses/Scope.h"
#include "MarineRunner/GunClasses/Components/GunControlsComponent.h"
#include "MarineRunner/GunClasses/Components/GunReloadComponent.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());
	
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

	bool bDrawGunAccordingToHandNumber = Player->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, Gun);

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