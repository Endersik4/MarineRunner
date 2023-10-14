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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	class AGun* GetGun() const { return Gun; }
	bool GetIsPlayerInAds() const { return bIsPlayerADS; }

	void SetGun(class AGun* NewGun) { Gun = NewGun; }
	void SetCanChangeWeapon(bool bCan) { bCanChangeWeapon = bCan; }

	void Shoot();
	void ReleasedShoot();
	void Reload();
	void Zoom(float WheelAxis);

	void ADSPressed();
	void ADSReleased();

	void UpdateWeaponInformationOnHud();

	void SelectWeaponFromQuickInventory(int32 HandNumber);
	void HideGunAndAddTheNewOne(class AGun* NewGun);

private:
	UPROPERTY(EditAnywhere, Category = "Set up Gun")
		TArray<FSettingSavedInJsonFile> MouseSensitivityWhenScope;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSInSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSOutSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* QuickSelectSound;

	//Gun
	int32 CurrentScopeIndex;
	class AGun* Gun;

	//Aiming
	bool bIsPlayerADS;

	//Weapon Inventory
	bool bCanChangeWeapon = true;

	void LoadSavedSettingsFromGameInstance();


	AMarineCharacter* MarinePawn;
		
};
