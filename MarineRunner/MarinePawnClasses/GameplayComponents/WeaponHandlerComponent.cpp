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

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	
	FTimerHandle UpdateMouseSensitivitesHandle;
	GetWorld()->GetTimerManager().SetTimer(UpdateMouseSensitivitesHandle, this, &UWeaponHandlerComponent::LoadSavedSettingsFromGameInstance, 0.05f, false);
}

#pragma region ////////////////////////////////// GUN //////////////////////////////////
void UWeaponHandlerComponent::ADSPressed()
{
	if (IsValid(Gun) == false) 
		return;

	if (Gun->GetGunReloadComponent()->GetIsReloading() && Gun->GetGunReloadComponent()->GetReloadOneBullet())
		Gun->GetGunReloadComponent()->CancelReload();

	if (Gun->GetCanShoot() == false) 
		return;

	MarinePawn->MakeCrosshire(true);
	MarinePawn->SetMovementForceDividerWhenInADS(MovementForceDividerWhenInADS);

	if (ADSInSound) 
		UGameplayStatics::SpawnSound2D(GetWorld(), ADSInSound);

	bIsPlayerADS = true;
	Gun->AimTheGun(EStatusOfAimedGun::ESAG_ADS);

	if (CurrentScopeIndex >= MouseSensitivityWhenScope.Num())
		return;

	MarinePawn->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}

void UWeaponHandlerComponent::ADSReleased()
{
	if (IsValid(Gun) == false || bIsPlayerADS == false) 
		return;

	MarinePawn->MakeCrosshire();
	MarinePawn->SetMovementForceDividerWhenInADS(1.f);

	if (ADSOutSound) UGameplayStatics::SpawnSound2D(GetWorld(), ADSOutSound);
	bIsPlayerADS = false;

	Gun->AimTheGun(EStatusOfAimedGun::ESAG_HipFire);

	MarinePawn->ChangeMouseSensitivity(FSettingSavedInJsonFile(), true);

	if (Gun->GetUseScope() == true)
		CurrentScopeIndex = Gun->GetScopeActor()->Zoom(0.f, true);
	else
		CurrentScopeIndex = 0;
}

void UWeaponHandlerComponent::UpdateWeaponInformationOnHud()
{
	if (IsValid(Gun) == false)
		return;
	
	Gun->GetGunControlsComponent()->UpdateWeaponDataInHud(true);
}

void UWeaponHandlerComponent::Shoot()
{
	if (IsValid(Gun) == false) 
		return;

	Gun->SetShootButtonPressed(true);
	Gun->Shoot();
}

void UWeaponHandlerComponent::ReleasedShoot()
{
	if (IsValid(Gun) == false) 
		return;

	Gun->ShootReleased();
}

void UWeaponHandlerComponent::Reload()
{
	if (IsValid(Gun) == false) 
		return;

	Gun->GetGunReloadComponent()->PrepareToReload();
}

void UWeaponHandlerComponent::Zoom(float WheelAxis)
{
	if (IsValid(Gun) == false || bIsPlayerADS == false || WheelAxis == 0.f) 
		return;

	if (Gun->GetUseScope() == false)
		return;

	if (IsValid(Gun->GetScopeActor()) == false)
		return;

	CurrentScopeIndex = Gun->GetScopeActor()->Zoom(WheelAxis);

	if (CurrentScopeIndex >= MouseSensitivityWhenScope.Num())
		return;

	MarinePawn->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}
#pragma endregion 

#pragma region ////////////////////// EQUIP WEAPON FROM INVENTORY //////////////////////
void UWeaponHandlerComponent::SelectWeaponFromQuickInventory(int32 HandNumber)
{
	if (bCanChangeWeapon == false || bIsPlayerADS == true)
		return;


	bool bDrawGunAccordingToHandNumber = MarinePawn->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, Gun);

	if (bDrawGunAccordingToHandNumber == false)
		return;

	bCanChangeWeapon = false;

}

void UWeaponHandlerComponent::DrawNewGun()
{
	if (IsValid(MarinePawn->GetWeaponInventoryComponent()->GetCurrentGunToDraw()) == false)
		return;

	Gun = MarinePawn->GetWeaponInventoryComponent()->GetCurrentGunToDraw();
	Gun->GetGunControlsComponent()->DrawGun();
}

void UWeaponHandlerComponent::DropGun()
{
	if (bCanChangeWeapon == false)
		return;

	if (IsValid(Gun) == false)
		return;

	Gun->GetGunControlsComponent()->SetDropGun(true);
	if (MarinePawn->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1)
		Gun->GetGunControlsComponent()->PutAwayGun();
	else 
		SelectWeaponFromQuickInventory(MarinePawn->GetWeaponInventoryComponent()->GetLastWeaponSlotFromStorage(Gun));
}

void UWeaponHandlerComponent::HideCurrentHoldingGun()
{
	if (IsValid(Gun) == false)
		return;

	Gun->GetGunControlsComponent()->HideGun();
}
#pragma endregion 

void UWeaponHandlerComponent::LoadSavedSettingsFromGameInstance()
{
	if (IsValid(MarinePawn) == false)
		return;

	UMarineRunnerGameInstance* GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	AMarinePlayerController* MarinePlayerController = Cast<AMarinePlayerController>(MarinePawn->GetController());

	if (IsValid(GameInstance) == false || IsValid(MarinePlayerController) == false)
		return;

	GameInstance->FindSavedValueAccordingToName(MarinePawn->GetMouseSensitivityJSON().FieldName, MarinePawn->GetMouseSensitivityJSON().FieldValue);
	const FSettingSavedInJsonFile& CurrentMouseSensName = MarinePlayerController->GetMouseSensitivity();
	if (CurrentMouseSensName == MarinePawn->GetMouseSensitivityJSON()) MarinePawn->ChangeMouseSensitivity(MarinePawn->GetMouseSensitivityJSON(), true);

	for (FSettingSavedInJsonFile& CurrSetting : MouseSensitivityWhenScope)
	{
		GameInstance->FindSavedValueAccordingToName(CurrSetting.FieldName, CurrSetting.FieldValue);
		if (CurrentMouseSensName == CurrSetting) MarinePawn->ChangeMouseSensitivity(CurrSetting);
	}
}