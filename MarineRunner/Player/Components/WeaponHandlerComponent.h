// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "WeaponHandlerComponent.generated.h"

class AWeaponBase;
DECLARE_DELEGATE_OneParam(FSelectWeaponDelegate, int32);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	FORCEINLINE const TArray<FSettingSavedInJsonFile> & GetMouseSensitivityWhenScope() const { return MouseSensitivityWhenScope; }

	FORCEINLINE void SetWeapon(TObjectPtr<AWeaponBase> NewGun) { CurrentWeapon = NewGun; }
	FORCEINLINE void SetCanChangeWeapon(bool bCan) { bCanChangeWeapon = bCan; }
	FORCEINLINE void SetIsPlayerInAds(bool bIs) { bIsPlayerADS = bIs; }

	void PrimaryAction();
	void ReleasedPrimaryAction();

	void ActionFromKey_One();
	void TertiaryAction(float WheelAxis);

	void SecondaryAction();
	void ReleasedSecondaryAction();

	void UpdateWeaponInformationOnHud();

	void SelectWeaponFromQuickInventory(int32 HandNumber);
	void HideCurrentHoldingWeapon();
	void DrawNewEquipedWeapon();
	void DropCurrentHoldingWeapon();

	void LoadSavedSettingsFromGameInstance();
private:
	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<FSettingSavedInJsonFile> MouseSensitivityWhenScope = 
	{
		FSettingSavedInJsonFile("MouseSensitivityADS", 0.4f),
		FSettingSavedInJsonFile("MouseSensitivity2x", 0.2f),
		FSettingSavedInJsonFile("MouseSensitivity4x", 0.1f),
		FSettingSavedInJsonFile("MouseSensitivity8x", 0.05f),
	};

	UPROPERTY(Transient)
		TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

	UPROPERTY(Transient)
		bool bIsPlayerADS = false;

	UPROPERTY(Transient)
		bool bCanChangeWeapon = true;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player = nullptr;
};
