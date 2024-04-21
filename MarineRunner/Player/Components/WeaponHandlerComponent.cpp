// Copyright Adam Bartela.All Rights Reserved

#include "WeaponHandlerComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/MarinePlayerController.h"
#include "MarineRunner/Player/Interfaces/WeaponInterface.h"

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
void UWeaponHandlerComponent::PrimaryAction()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->PrimaryAction();
}

void UWeaponHandlerComponent::ReleasedPrimaryAction()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->ReleasedPrimaryAction();
}

void UWeaponHandlerComponent::ActionFromKey_One()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->ActionFromKey_One();
}

void UWeaponHandlerComponent::SecondaryAction()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->SecondaryAction();
}

void UWeaponHandlerComponent::ReleasedSecondaryAction()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->ReleasedSecondaryAction();
}

void UWeaponHandlerComponent::UpdateWeaponInformationOnHud()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->UpdateWeaponHudInformation();
}

void UWeaponHandlerComponent::TertiaryAction(float WheelAxis)
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->TertiaryAction(WheelAxis);
}
#pragma endregion 

#pragma region ////////////////////// EQUIP WEAPON FROM INVENTORY //////////////////////
void UWeaponHandlerComponent::SelectWeaponFromQuickInventory(int32 HandNumber)
{
	if (!bCanChangeWeapon|| bIsPlayerADS)
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

	if (!CurrentWeapon)
		return;

	CurrentWeapon->SetDropWeapon(true);
	if (Player->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1)
		CurrentWeapon->PutAwayWeapon();
	else 
		SelectWeaponFromQuickInventory(Player->GetWeaponInventoryComponent()->GetLastWeaponSlotFromStorage(CurrentWeapon));
}

void UWeaponHandlerComponent::HideCurrentHoldingWeapon()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->HideWeapon();
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