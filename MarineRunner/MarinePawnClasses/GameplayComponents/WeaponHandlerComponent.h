// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "WeaponHandlerComponent.generated.h"


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

	class AGun* GetGun() const { return Gun; }
	bool GetIsPlayerInAds() const { return bIsPlayerADS; }

	FORCEINLINE void SetGun(class AGun* NewGun) { Gun = NewGun; }
	FORCEINLINE void SetCanChangeWeapon(bool bCan) { bCanChangeWeapon = bCan; }

	void Shoot();
	void ReleasedShoot();
	void Reload();
	void Zoom(float WheelAxis);

	void ADSPressed();
	void ADSReleased();

	void UpdateWeaponInformationOnHud();

	void SelectWeaponFromQuickInventory(int32 HandNumber);
	void HideCurrentHoldingGun();
	void DrawNewGun();
	void DropGun();

private:
	UPROPERTY(EditAnywhere, Category = "Set up Gun")
		TArray<FSettingSavedInJsonFile> MouseSensitivityWhenScope = 
	{
		FSettingSavedInJsonFile("MouseSensitivity2x", 0.4f),
		FSettingSavedInJsonFile("MouseSensitivity4x", 0.2f),
		FSettingSavedInJsonFile("MouseSensitivity8x", 0.1f),
		FSettingSavedInJsonFile("MouseSensitivity16x", 0.05f),
	};

	UPROPERTY(EditDefaultsOnly, Category = "ADS SEttings")
		float MovementForceDividerWhenInADS = 1.5f;


	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSInSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSOutSound;

	//Gun
	int32 CurrentScopeIndex;
	class AGun* Gun;

	//Aiming
	bool bIsPlayerADS;

	//Weapon Inventory
	bool bCanChangeWeapon = true;

	void LoadSavedSettingsFromGameInstance();

	class AMarineCharacter* MarinePawn;	
};
