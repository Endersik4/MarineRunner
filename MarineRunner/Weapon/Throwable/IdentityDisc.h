// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/Weapon/WeaponBase.h"

#include "IdentityDisc.generated.h"

UCLASS()
class MARINERUNNER_API AIdentityDisc : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	AIdentityDisc();

protected:
	virtual void BeginPlay() override;

	virtual void PickUpWeaponItem(class AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad) override;

	virtual void DrawWeapon() override;
	virtual void PutAwayWeapon() override;
	virtual void HideWeapon() override;
	virtual void DropWeapon() override;

	virtual void PrimaryAction() override;
	virtual void ReleasedPrimaryAction() override;
	virtual void SecondaryAction() override;
	virtual void ReleasedSecondaryAction() override;
	virtual void TertiaryAction(float Value) override;

	virtual void ActionFromKey_One() override;

	virtual void UpdateWeaponHudInformation(bool bUpdateStoredAmmoText = false, bool bUpdateWeaponImage = false) override;
	virtual int32 GetIntValueToSave() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings")
	float FullPowerDamage = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings")
	float NormalDamage = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings")
	float TimeToFullyRecharge = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings")
	float TimeToThrowDisc = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings|Animation")
	FWeaponAnimation ThrowDiscAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings|Animation")
	FWeaponAnimation CatchDiscAnim_Start;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings|Animation")
	FWeaponAnimation CatchDiscAnim_Middle;
	UPROPERTY(EditDefaultsOnly, Category = "Disc Settings|Animation")
	FWeaponAnimation CatchDiscAnim_End;

	void ThrowDisc();
};
