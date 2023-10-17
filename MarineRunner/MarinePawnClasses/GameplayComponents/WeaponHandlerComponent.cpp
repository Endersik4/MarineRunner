// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/WeaponHandlerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MarineRunner/GunClasses/Gun.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/MarinePawnClasses/WeaponInventoryComponent.h"
#include "MarineRunner/MarinePawnClasses/MarinePlayerController.h"


// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();

	MarinePawn = Cast<AMarineCharacter>(GetOwner());
	
	LoadSavedSettingsFromGameInstance();
}


// Called every frame
void UWeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

#pragma region ////////////////////////////////// GUN //////////////////////////////////
void UWeaponHandlerComponent::ADSPressed()
{
	if (IsValid(Gun) == false) return;
	if (Gun->GetIsGrabbingEnded() == false || Gun->GetIsReloading()) return;

	MarinePawn->MakeCrosshire(true);

	if (ADSInSound) UGameplayStatics::SpawnSound2D(GetWorld(), ADSInSound);

	bIsPlayerADS = true;
	Gun->SetStatusOfGun(StatusOfAimedGun::ADS);
	if (Gun->GetShouldChangeMouseSensitivityADS() == true) MarinePawn->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}

void UWeaponHandlerComponent::ADSReleased()
{
	if (IsValid(Gun) == false || bIsPlayerADS == false) return;

	MarinePawn->MakeCrosshire();

	if (ADSOutSound) UGameplayStatics::SpawnSound2D(GetWorld(), ADSOutSound);
	bIsPlayerADS = false;
	Gun->SetStatusOfGun(StatusOfAimedGun::BackToInitialPosition);
	if (Gun->GetShouldChangeMouseSensitivityADS() == true)
	{
		MarinePawn->ChangeMouseSensitivity(FSettingSavedInJsonFile(), true);

		CurrentScopeIndex = Gun->ZoomScope(0.f, true);
	}
}

void UWeaponHandlerComponent::UpdateWeaponInformationOnHud()
{
	if (IsValid(Gun) == false)
		return;
	
	Gun->UpdateWeaponDataInHud(true);
}

void UWeaponHandlerComponent::Shoot()
{
	if (IsValid(Gun) == false) return;

	Gun->Shoot();
}

void UWeaponHandlerComponent::ReleasedShoot()
{
	if (IsValid(Gun) == false) return;

	Gun->ShootReleased();
}

void UWeaponHandlerComponent::Reload()
{
	if (IsValid(Gun) == false) return;

	Gun->WaitToReload();
}

void UWeaponHandlerComponent::Zoom(float WheelAxis)
{
	if (IsValid(Gun) == false || bIsPlayerADS == false || WheelAxis == 0.f) 
		return;

	CurrentScopeIndex = Gun->ZoomScope(WheelAxis);
	MarinePawn->ChangeMouseSensitivity(MouseSensitivityWhenScope[CurrentScopeIndex]);
}
#pragma endregion 

#pragma region ////////////////////// EQUIP WEAPON FROM INVENTORY //////////////////////
void UWeaponHandlerComponent::SelectWeaponFromQuickInventory(int32 HandNumber)
{
	if (!bCanChangeWeapon)
		return;
	if (MarinePawn->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, Gun) == Gun)
		return;

	Gun = MarinePawn->GetWeaponInventoryComponent()->GetWeaponFromStorage(HandNumber, Gun);
	if (QuickSelectSound) UGameplayStatics::SpawnSound2D(GetWorld(), QuickSelectSound);
}

void UWeaponHandlerComponent::HideGunAndAddTheNewOne(AGun* NewGun)
{
	if (IsValid(Gun))
	{
		Gun->SetActorHiddenInGame(true);
		Gun->SetGunSwayWhileMovingTimer(true);
		Gun->ShootReleased();
	}
	MarinePawn->GetWeaponInventoryComponent()->AddNewWeaponToStorage(NewGun);
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

	const FSettingSavedInJsonFile& CurrentMouseSensName = MarinePlayerController->GetMouseSensitivity();

	for (FSettingSavedInJsonFile& CurrSetting : MouseSensitivityWhenScope)
	{
		GameInstance->SetValueBySavedSettingName(CurrSetting.FieldName, CurrSetting.FieldValue);
		if (CurrentMouseSensName == CurrSetting) MarinePawn->ChangeMouseSensitivity(CurrSetting);
	}
}