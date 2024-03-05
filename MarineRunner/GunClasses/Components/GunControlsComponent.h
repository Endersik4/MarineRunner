// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunControlsComponent.generated.h"

// Handles all gun controls: Take, Put Away, Draw, Drop, Hud
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UGunControlsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGunControlsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void TakeGun(TObjectPtr<class AMarineCharacter> NewPlayer, bool bWasTaken = false, int32 SavedMagazineCapacity = 0);
	void DrawGun();
	void PutAwayGun();
	void HideGun();

	void UpdateWeaponDataInHud(bool bUpdateStoredAmmoText = false, bool bUpdateWeaponImage = false);

	FORCEINLINE void SetDropGun(bool bDrop) { bDropGun = bDrop; }

	FORCEINLINE const FName & GetAttachToSocketName() const { return AttachGunToSocketName; }
	struct FItemStruct* GetAmmunitionFromInventory() const;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Take gun")
		FName AttachGunToSocketName = FName(TEXT("Weapon_R"));
	UPROPERTY(EditDefaultsOnly, Category = "Take gun")
		struct FWeaponAnimation WeaponFirstTimeTakeAnim;

	// If True then ammunition on UI will be below the Gun icon picture
	// If False then ammunition on UI will be on the left side of the gun icon picture
	UPROPERTY(EditAnywhere, Category = "Gun Hud")
		bool bAmmoCounterBelowGunHUD;
	UPROPERTY(EditDefaultsOnly, Category = "Gun Hud")
		TObjectPtr<UTexture2D> GunHUDTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Handle Gun")
		struct FWeaponAnimation WeaponDrawAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Handle Gun")
		TObjectPtr<USoundBase> DrawGunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Handle Gun")
		struct FWeaponAnimation WeaponPutAwayAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Handle Gun")
		TObjectPtr<USoundBase> PutAwayGunSound;

	UPROPERTY(EditDefaultsOnly, Category = "Drop gun")
		TSubclassOf<class APickupItem> ItemToSpawnAfterDropGun;
	UPROPERTY(EditDefaultsOnly, Category = "Drop gun")
		float DistanceToDropGun = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Drop gun")
		TObjectPtr<USoundBase> DropGunSound;

	//The item that is the ammunition for this weapon.
	UPROPERTY(EditDefaultsOnly, Category = "Stored Ammunition")
		FName RowNameForAmmunitionItem;
	//When a player picks up a weapon for the first time, StoredAmmo will be added to the inventory.
	UPROPERTY(EditAnywhere, Category = "Stored Ammunition")
		int32 StoredAmmo = 50;

	bool bDropGun;
	void DropGun();

	void AddAmmoToInventory();

	UPROPERTY(Transient)
		TObjectPtr<class AGun> Gun;
	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
};
