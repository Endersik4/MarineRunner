// Copyright Adam Bartela.All Rights Reserved

#include "WeaponHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Weapon/WeaponBase.h"

UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Weapon Handler Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}
	
	FTimerHandle UpdateMouseSensitivitesHandle;
	GetWorld()->GetTimerManager().SetTimer(UpdateMouseSensitivitesHandle, this, &UWeaponHandlerComponent::LoadSavedMouseSensitivities, TimeToLoadMouseSensitivity, false);
}

#pragma region ////////////////////////////////// WEAPON HANDLING //////////////////////////////////
void UWeaponHandlerComponent::PrimaryAction()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->PrimaryAction();
}

void UWeaponHandlerComponent::ReleasedPrimaryAction()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->ReleasedPrimaryAction();
}

void UWeaponHandlerComponent::ActionFromKey_One()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->ActionFromKey_One();
}

void UWeaponHandlerComponent::SecondaryAction()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->SecondaryAction();
}

void UWeaponHandlerComponent::ReleasedSecondaryAction()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->ReleasedSecondaryAction();
}

void UWeaponHandlerComponent::UpdateWeaponInformationOnHud()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->UpdateWeaponHudInformation(true, true);
}

void UWeaponHandlerComponent::TertiaryAction(float WheelAxis)
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->TertiaryAction(WheelAxis);
}
#pragma endregion 

#pragma region ////////////////////// EQUIP WEAPON FROM INVENTORY //////////////////////
void UWeaponHandlerComponent::SelectWeaponFromQuickInventory(int32 HandNumber)
{
	if (!bCanChangeWeapon || bIsPlayerADS)
		return;

	const bool bDrawWeaponAccordingToHandNumber = Player->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, CurrentWeapon);

	if (!bDrawWeaponAccordingToHandNumber)
		return;

	bCanChangeWeapon = false;
}

void UWeaponHandlerComponent::DrawNewEquipedWeapon()
{
	if (!Player->GetWeaponInventoryComponent()->GetCurrentWeaponToDraw())
		return;

	CurrentWeapon = Player->GetWeaponInventoryComponent()->GetCurrentWeaponToDraw();
	CurrentWeapon->DrawWeapon();
}

void UWeaponHandlerComponent::DropCurrentHoldingWeapon()
{
	if (!bCanChangeWeapon)
		return;

	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->SetDropWeapon(true);

	if (Player->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1)
		CurrentWeapon->PutAwayWeapon();
	else 
		SelectWeaponFromQuickInventory(Player->GetWeaponInventoryComponent()->GetLastWeaponSlotFromStorage(CurrentWeapon));
}

void UWeaponHandlerComponent::HideCurrentHoldingWeapon()
{
	if (!IsValid(CurrentWeapon))
		return;

	CurrentWeapon->HideWeapon();
}
#pragma endregion 

void UWeaponHandlerComponent::LoadSavedMouseSensitivities()
{
	if (!IsValid(Player))
		return;

	TObjectPtr<UMarineRunnerGameInstance> GameInstance = Cast<UMarineRunnerGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	TObjectPtr<AMarinePlayerController> MarinePlayerController = Cast<AMarinePlayerController>(Player->GetController());

	if (!IsValid(GameInstance) || !IsValid(MarinePlayerController))
		return;

	// updates player's default mouse sensitivity
	GameInstance->FindSavedValueAccordingToName(Player->GetMouseSensitivityJSON().FieldName, Player->GetMouseSensitivityJSON().FieldValue);
	
	const FSettingSavedInJsonFile& CurrentMouseSensName = MarinePlayerController->GetMouseSensitivity();

	// If current mouse sensitivity name is the same as player's default then update mouse sensitivity value
	if (CurrentMouseSensName == Player->GetMouseSensitivityJSON()) 
		Player->ChangeMouseSensitivity(Player->GetMouseSensitivityJSON(), true);

	for (FSettingSavedInJsonFile& CurrSetting : MouseSensitivityWhenScope)
	{
		// updates remaining mouse sensitivities
		GameInstance->FindSavedValueAccordingToName(CurrSetting.FieldName, CurrSetting.FieldValue);

		if (CurrentMouseSensName == CurrSetting) 
			Player->ChangeMouseSensitivity(CurrSetting);
	}
}