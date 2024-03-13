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

	FORCEINLINE TObjectPtr<class AGun> GetGun() const { return Gun; }
	FORCEINLINE bool GetIsPlayerInAds() const { return bIsPlayerADS; }

	FORCEINLINE void SetGun(TObjectPtr<class AGun> NewGun) { Gun = NewGun; }
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

	void LoadSavedSettingsFromGameInstance();
private:
	UPROPERTY(EditAnywhere, Category = "Gun")
		TArray<FSettingSavedInJsonFile> MouseSensitivityWhenScope = 
	{
		FSettingSavedInJsonFile("MouseSensitivityADS", 0.4f),
		FSettingSavedInJsonFile("MouseSensitivity2x", 0.2f),
		FSettingSavedInJsonFile("MouseSensitivity4x", 0.1f),
		FSettingSavedInJsonFile("MouseSensitivity8x", 0.05f),
	};

	UPROPERTY(EditDefaultsOnly, Category = "Gun|Ads")
		float MovementForceDividerWhenInADS = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Ads")
		TObjectPtr<USoundBase> ADSInSound;
	UPROPERTY(EditDefaultsOnly, Category = "Gun|Ads")
		TObjectPtr<USoundBase> ADSOutSound;

	//Gun
	int32 CurrentScopeIndex;
	UPROPERTY(Transient)
		TObjectPtr<class AGun> Gun;

	bool bIsPlayerADS;

	bool bCanChangeWeapon = true;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;	
};
