// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/Weapon/Gun/Bullet/BulletData.h"
#include "MarineRunner/Weapon/WeaponBase.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum EStatusOfAimedGun
{
	ESAG_ADS UMETA(DisplayName = "ADS"),
	ESAG_HipFire UMETA(DisplayName = "Hip-Fire"),
};

UCLASS()
class MARINERUNNER_API AGun : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PickUpWeaponItem(class AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad) override;

	virtual void DrawWeapon() override;
	virtual void PutAwayWeapon() override;
	virtual void HideWeapon() override;
	virtual void DropWeapon() override;

	// Start Shooting
	virtual void PrimaryAction() override;
	// Stop Shooting
	virtual void ReleasedPrimaryAction() override;
	// Begin Ads
	virtual void SecondaryAction() override;
	// End Ads
	virtual void ReleasedSecondaryAction() override;
	// Zoom (if possible)
	virtual void TertiaryAction(float Value) override;
	// Reload
	virtual void ActionFromKey_One() override;
	// Get Current Magazine Capacity
	virtual int32 GetIntValueToSave() override;
public:
	virtual void UpdateWeaponHudInformation(bool bUpdateStoredAmmoText = false, bool bUpdateWeaponImage = false) override;
	struct FItemStruct* GetAmmunitionFromInventory() const;

	void ShootReleased();

	FORCEINLINE void SetCanShoot(bool bCan) { bCanShoot = bCan; }
	FORCEINLINE void SetConstantlyShoot(bool bNewConstantlyShoot) { bConstantlyShoot = bNewConstantlyShoot; }

	FORCEINLINE const bool GetCanShoot() const { return bCanShoot; }
	FORCEINLINE float GetShootTime() const { return ShootTime; }
	FORCEINLINE EStatusOfAimedGun GetStatusOfGun() const { return StatusOfGun; }

	FORCEINLINE TObjectPtr<class UGunReloadComponent> GetGunReloadComponent() const { return GunReloadComponent; }
	FORCEINLINE TObjectPtr<class AMarineCharacter> GetPlayer() const { return Player; }

	void AimTheGun(EStatusOfAimedGun NewGundStatus);
private:

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UGunRecoilComponent> GunRecoilComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UDropCasingComponent> DropCasingComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UGunReloadComponent> GunReloadComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
	FBulletStruct BulletData = FBulletStruct();
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
	bool bAutomaticGun = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
	bool bManyBulletsPerShoot = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun", meta = (EditCondition = "bManyBulletsPerShoot", EditConditionHides))
	int32 BulletsAmountPerShoot = 10;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Shoot")
	float ShootTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot")
	float CoyoteShootTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot")
	FName AttachShootEffectsSocketName = FName(TEXT("MuzzleFlash"));
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Animations")
	FWeaponAnimation WeaponShootAnim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Animations")
	FWeaponAnimation WeaponShootWithNoBulletsAnim = FWeaponAnimation();;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Animations")
	FWeaponAnimation WeaponADSShootAnim = FWeaponAnimation();;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Animations")
	FWeaponAnimation WeaponADSShootWithNoBulletsAnim = FWeaponAnimation();;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Sounds")
	TObjectPtr<USoundBase> ShootSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Sounds")
	TObjectPtr<USoundBase> EmptyMagazineSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Particles")
	TObjectPtr<UParticleSystem> ShootParticle = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Shoot|Particles")
	float ShootParticleScale = 1.f;

	//The item that is the ammunition for this weapon.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammo")
	FName RowNameForAmmunitionItem = FName();
	//When a player picks up a weapon for the first time, StoredAmmo will be added to the inventory.
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Ammo")
	int32 StoredAmmo = 50;

	// If True then ammunition on UI will be below the Gun icon picture
	// If False then ammunition on UI will be on the left side of the gun icon picture
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Hud")
	bool bAmmoCounterBelowGunHUD = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Hud")
	TObjectPtr<UTexture2D> GunHUDTexture = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Scope")
	bool bUseScope = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Scope", meta = (EditCondition = "bUseScope", EditConditionHides))
	int32 ZoomMaterialIndexOnWeapon = 0;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
	float MovementSpeedWhenInADS = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
	float WeaponSwayInADSDivider = 3.5f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
	float WeaponSwayWhileMovingInADSDivider = 3.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|ADS")
	float MovementForceDividerWhenInADS = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|ADS|Sounds")
	TObjectPtr<USoundBase> ADSInSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|ADS|Sounds")
	TObjectPtr<USoundBase> ADSOutSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|ADS|Animations")
	FWeaponAnimation WeaponADSInAnim = FWeaponAnimation();;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|ADS|Animations")
	FWeaponAnimation WeaponADSOutAnim = FWeaponAnimation();;

	UPROPERTY(Transient)
	bool bShootButtonPressed = false;
	UPROPERTY(Transient)
	TEnumAsByte<EStatusOfAimedGun> StatusOfGun = EStatusOfAimedGun::ESAG_HipFire;

	// Shoot
	FTimerHandle ShootHandle;
	UPROPERTY(Transient)
	bool bCanShoot = true;
	bool CanShoot();
	void Shoot();
	void ShootFinished();
	void SetCanShootAgain();
	void CancelShoot();

	void SpawnBullet();

	void AddStoredAmmoToInventory();

	// Gun Effects
	void PlayGunShootAnimation();
	bool NoBulletsShootAnim();
	void AddEffectsToShooting();

	// Constantly Shooting 
	UPROPERTY(Transient)
	bool bConstantlyShoot = false;
	FTimerHandle ConstantlyShootHandle;

	UPROPERTY(Transient)
	bool bCoyoteShootTime = false;
	FORCEINLINE void ShootAfterDelayIfCan() { bCoyoteShootTime = false; }
	FTimerHandle CoyoteShootHandle;

	// Scope
	void SetUpZoom();
	UPROPERTY(Transient)
	int32 CurrentScopeIndex = 0;
	UPROPERTY(Transient)
	TObjectPtr<class AScope> ScopeActor = nullptr;
};