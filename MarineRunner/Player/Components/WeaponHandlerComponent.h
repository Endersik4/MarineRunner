// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "WeaponHandlerComponent.generated.h"

class AWeaponBase;
DECLARE_DELEGATE_OneParam(FSelectWeaponDelegate, int32);


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UWeaponHandlerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponHandlerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	FORCEINLINE bool GetIsPlayerInAds() const { return bIsPlayerADS; }
	FORCEINLINE TObjectPtr<AWeaponBase> GetCurrentWeapon() const { return CurrentWeapon; }
	FORCEINLINE bool GetIsWeaponHiddenByPlayer() const { return bWeaponHiddenByPlayer; }
	FORCEINLINE const TArray<FSettingSavedInJsonFile>& GetMouseSensitivityWhenScope() const { return MouseSensitivityWhenScope; }

	FORCEINLINE void SetWeapon(TObjectPtr<AWeaponBase> NewGun) { CurrentWeapon = NewGun; }
	FORCEINLINE void SetCanChangeWeapon(bool bCan) { bCanChangeWeapon = bCan; }
	FORCEINLINE void SetWeaponHiddenByPlayer(bool bHide) { bWeaponHiddenByPlayer = bHide; }
	FORCEINLINE void SetIsPlayerInAds(bool bIs) { bIsPlayerADS = bIs; }

	// e.g LMB = Shoot (Gun)
	void PrimaryAction();
	void ReleasedPrimaryAction();

	// e.g R = Reload (Gun)
	void ActionFromKey_One();
	// e.g Mouse Scroll = Zoom (Gun)
	void TertiaryAction(float WheelAxis);

	// e.g RMB = ADS 
	void SecondaryAction();
	void ReleasedSecondaryAction();

	void UpdateWeaponInformationOnHud();

	void SelectWeaponFromQuickInventory(int32 HandNumber);
	void HideCurrentHoldingWeapon();
	void DrawNewEquipedWeapon();
	void DropCurrentHoldingWeapon();

	void CallHideWeaponByPlayer();
	// returns hide anim time
	float HideWeaponByPlayer();

	void LoadSavedMouseSensitivities();


private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<FSettingSavedInJsonFile> MouseSensitivityWhenScope =
	{
		FSettingSavedInJsonFile("MouseSensitivityADS", 0.4f),
		FSettingSavedInJsonFile("MouseSensitivity2x", 0.2f),
		FSettingSavedInJsonFile("MouseSensitivity4x", 0.1f),
		FSettingSavedInJsonFile("MouseSensitivity8x", 0.05f),
	};
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TimeToLoadMouseSensitivity = 0.05f;

	UPROPERTY(Transient)
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

	UPROPERTY(Transient)
	bool bIsPlayerADS = false;

	UPROPERTY(Transient)
	bool bCanChangeWeapon = true;

	UPROPERTY(Transient)
	bool bWeaponHiddenByPlayer = false;

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
};
