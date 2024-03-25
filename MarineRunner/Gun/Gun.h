// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Gun/Bullet/BulletData.h"
#include "Gun.generated.h"

UENUM(BlueprintType)
enum EStatusOfAimedGun
{
	ESAG_ADS UMETA(DisplayName = "ADS"),
	ESAG_HipFire UMETA(DisplayName = "Hip-Fire"),
};

USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		UAnimationAsset* WeaponActionAnim;
	UPROPERTY(EditAnywhere)
		UAnimationAsset* ArmsActionAnim;

	FWeaponAnimation()
	{
		WeaponActionAnim = nullptr;
		ArmsActionAnim = nullptr;
	}

	FORCEINLINE bool IsWeaponAnimationValid() const {
		return WeaponActionAnim && ArmsActionAnim;
	}
};

UCLASS()
class MARINERUNNER_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	void Shoot();
	void ShootReleased();
	void SetCanShootAgain();

	void CancelShoot();

	FORCEINLINE void SetCanShoot(bool bCan) { bCanShoot = bCan; }
	FORCEINLINE void SetShootButtonPressed(bool bPressed) { bShootButtonPressed = bPressed; }
	FORCEINLINE void SetConstantlyShoot(bool bNewConstantlyShoot) { bConstantlyShoot = bNewConstantlyShoot; }
	FORCEINLINE void SetPlayer(TObjectPtr<class AMarineCharacter> NewPlayer) { Player = NewPlayer; }

	FORCEINLINE const bool GetCanShoot() const { return bCanShoot; }
	FORCEINLINE float GetShootTime() const { return ShootTime; }
	FORCEINLINE EStatusOfAimedGun GetStatusOfGun() const { return StatusOfGun; }

	FORCEINLINE bool GetUseScope() const { return bUseScope; }
	FORCEINLINE TObjectPtr<class AScope> GetScopeActor() const { return ScopeActor; }

	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetGunSkeletalMesh() const { return GunSkeletalMesh; }
	FORCEINLINE TObjectPtr<class UGunControlsComponent> GetGunControlsComponent() const { return GunControlsComponent; }
	FORCEINLINE TObjectPtr<class UGunReloadComponent> GetGunReloadComponent() const { return GunReloadComponent; }
	FORCEINLINE TObjectPtr<class AMarineCharacter> GetPlayer() const { return Player; }

	void AimTheGun(EStatusOfAimedGun NewGundStatus);

	void PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<USkeletalMeshComponent> GunSkeletalMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UGunRecoilComponent> GunRecoilComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UDropCasingComponent> DropCasingComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UGunControlsComponent> GunControlsComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UGunReloadComponent> GunReloadComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		FBulletStruct BulletData = FBulletStruct();

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

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bAutomaticGun = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bManyBulletsPerShoot = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun", meta = (EditCondition = "bManyBulletsPerShoot", EditConditionHides))
		int32 BulletsAmountPerShoot = 10;

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
	void ShootFinished();

	void SpawnBullet();

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
		TObjectPtr<class AScope> ScopeActor = nullptr;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player = nullptr;
};