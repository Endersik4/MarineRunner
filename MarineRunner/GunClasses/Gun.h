// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "BulletData.h"

#include "Gun.generated.h"

UENUM(BlueprintType)
enum EStatusOfAimedGun
{
	ADS UMETA(DisplayName = "ADS"),
	HipFire UMETA(DisplayName = "Hip-Fire"),
	BackToInitialPosition UMETA(DisplayName = "Original Position"),
};

UENUM(BlueprintType)
enum EWhenSpawnCasing
{
	EWSC_WhileShooting UMETA(DisplayName = "WhileShooting"),
	EWSC_WhileReloading UMETA(DisplayName = "WhileReloading"),
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TakeGun(class AMarineCharacter* Player, bool bWasTaken = false, int32 SavedMagazineCapacity = 0);
	void DrawGun();
	void PutAwayGun();
	void HideGun();

	void WaitToReload();
	void CancelReload();

	void Shoot();
	void ShootReleased();

	FORCEINLINE bool GetCanShoot() const { return bCanShoot; }
	FORCEINLINE bool GetShouldChangeMouseSensitivityADS() const { return bShouldChangeMouseSensitivityADS; }
	FORCEINLINE bool GetIsReloading() const { return bIsReloading; }
	FORCEINLINE bool GetReloadOneBullet() const { return bReloadOneBullet; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	FORCEINLINE bool GetUseScope() const { return bUseScope; }
	FORCEINLINE class AScope* GetScopeActor() const { return ScopeActor; }
	FORCEINLINE USkeletalMeshComponent* GetGunSkeletalMesh() const { return GunSkeletalMesh; }
	FORCEINLINE FName GetAttachToSocketName() const { return AttachToSocketName; }

	FORCEINLINE void SetDropGun(bool bDrop) { bDropGun = bDrop; }

	void AimTheGun(EStatusOfAimedGun NewGundStatus);

	void UpdateWeaponDataInHud(bool bChangeStoredAmmoText = false, bool bChangeWeaponImage = false);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* GunSkeletalMesh;

	UFUNCTION()
		void ShootRecoilTimelineProgress(float x);

	UFUNCTION()
		void RecoilCameraTimelineCallback(float x);

	UFUNCTION()
		void RecoilCameraTimelineFinishedCallback() {};

	UPROPERTY(EditDefaultsOnly, Category = "Take gun")
		FName AttachToSocketName = FName(TEXT("Weapon_R"));

	UPROPERTY(EditDefaultsOnly, Category = "Drop gun")
		TSubclassOf<class APickupItem> ItemToSpawnAfterDropGun;
	UPROPERTY(EditDefaultsOnly, Category = "Drop gun")
		float DistanceToDropGun = 500.f;

	//General Damage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		FBulletStruct BulletData;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float ShootTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bReloadOneBullet;
	//Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bManyBulletAtOnce;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		int32 HowManyBulletsToSpawn = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Scope")
		bool bUseScope;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Scope", meta = (EditCondition = "bUseScope", EditConditionHides))
		int32 ZoomMaterialIndexOnWeapon;

	//The item that will be the ammunition for this weapon.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammunition")
		FName RowNameForAmmunitionItem;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Ammunition")
		int32 MagazineCapacity = 10;
	//When a player picks up a weapon for the first time, the value will be added to the inventory.
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Ammunition")
		int32 StoredAmmo = 50;

	// If True then ammunition on UI will be below the Gun icon picture and its 460x260
	// Useful for longer weapons
	// If False then ammunition on UI will be on the left side of the gun icon picture and its 260x150
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|UI")
		bool bAmmoCounterBelowGunHUD;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|UI")
		UTexture2D* GunHUDTexture;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		bool bShouldChangeMouseSensitivityADS = false;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float MovementForceWhenInADS = 50000.f;
	//This number will be subdivided with value from Recoil (camera, gun recoil)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfRecoilWhileADS = 3.5f;
	//This number will be subdivided with value from Recoil (bullet)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfBulletRecoilWhileADS = 3.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float WeaponSwayInADSDivider = 3.5f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float WeaponSwayWhileMovingInADSDivider = 3.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|DelayShoot")
		float DelayShootTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Recoil|Bullet")
		FFloatRange PitchBulletRecoilArray = FFloatRange(-5, 5);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Recoil|Bullet")
		FFloatRange YawBulletRecoilArray = FFloatRange(-5, 5);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection")
		bool bCasingEjection = true;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		TEnumAsByte<EWhenSpawnCasing> WhenSpawnCasing = EWhenSpawnCasing::EWSC_WhileShooting;
	// after shooting use timer to spawn casing 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		float SpawnCasingAfterTime = 0.05f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		bool bShouldRandomizeRotationOfCasing = true;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Roll = FFloatRange(-10.f, 10.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Pitch = FFloatRange(-15.f, 15.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Yaw = FFloatRange(-40.f, 10.f);
	//Actor that will spawn on the location from Socket "BulletDrop". Its for casing that is dumped from gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		TSubclassOf<AActor> DropBulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil")
		TSubclassOf<class UCameraShakeBase> ShootCameraShake;
	//Use curve for Camera Recoil or use random Pitch and Yaw numbers for camera Recoil
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil")
		bool bShouldUseCurveRecoil;
	//Speed of Interp, after shooting there camera will be back to position before shoting and this is the speed of it
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float InitalCameraPositionSpeed = 5.f;
	//Distance that Camera is going to rotate in pitch when shooting full magazine. 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float DistanceFromStart = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float FirstBulletWithoutRecoilTime = 1.f;
	//How long timeline need to least. It has to be the same like is in RecoilCameraCurveY
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float RecoilCameraTimelineLength = 3.2f;
	//Curve that is responisble for Yaw Camera Recoil (left, right)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		UCurveFloat* RecoilCameraCurveY;
	//Takes Value from RecoilCameraCurveY and randomize it with value from TArray<>. First Number in TArray is The Min and Second is The Max.
	//If TArray<> has more then 2 values then Value will not be randomize
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		FFloatRange RandomRangeFromRecoilCurveY = FFloatRange(-0.2f, 0.2f);

	//The curve responsible for how quickly the screen reaches PitchRecoilRandomNumber and YawRecoilRandomNumber and returns to its rotation. 
	//It was added for smoothness. It should have a length like the one in Animation|Setting Up Animation Recoil in RecoilAnimTimelineLength 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		UCurveFloat* RecoilCameraCurveRandomRotation;
	//Range for random Pitch number. 0 index has to be MinNumber and 1 index has to be MaxNumber
	//0 and 1 is only for positive side (UP)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		FFloatRange PitchRecoilRangeArray = FFloatRange(20, 10);
	//Range for random Yaw number. 0 index has to be MinNumber and 1 index has to be MaxNumber.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Camera Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		FFloatRange YawRecoilRangeArray = FFloatRange(18, 10);

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Shoot")
		FWeaponAnimation WeaponShootAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Shoot")
		FWeaponAnimation WeaponShootWithNoBulletsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Shoot")
		FWeaponAnimation WeaponADSShootAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Shoot")
		FWeaponAnimation WeaponADSShootWithNoBulletsAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Reload")
		FWeaponAnimation WeaponReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Reload")
		FWeaponAnimation WeaponReloadWithNoBulletsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Reload", meta = (EditCondition = "bReloadOneBullet"))
		FWeaponAnimation WeaponReload_Begin;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Reload", meta = (EditCondition = "bReloadOneBullet"))
		FWeaponAnimation WeaponReload_BeginEnd;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Reload", meta = (EditCondition = "bReloadOneBullet"))
		FWeaponAnimation WeaponReload_End;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Handle")
		FWeaponAnimation WeaponFirstTimeTakeAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Handle")
		FWeaponAnimation WeaponDrawAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Handle")
		FWeaponAnimation WeaponHideAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Handle")
		FWeaponAnimation WeaponADSInAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Handle")
		FWeaponAnimation WeaponADSOutAnim;


	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ShootingSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* EmptyMagazineSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* DrawGunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PutAwayGunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* DropGunSound;

	float OriginalPlayerFOV = 90.f;
	
	////////////// SHOOT ////////////
	bool bCanShoot = true;
	bool CanShoot();
	void ShootFinished();
	void SetCanShoot();
	////////////////////////////////

	////////////// BULLET ///////////
	FRotator BulletRotation;
	void RandomBulletDirection(FRotator& NewBulletRotation);
	void SpawnBullet();
	///////////////////////////////

	///////////// GUN EFFECTS ///////
	void PlayGunShootAnimation();
	bool NoBulletsShootAnim();
	void AddEffectsToShooting();
	void StartTimerToDropCasing(const EWhenSpawnCasing & CurrentSpawnCasingPeriod);
	void DropCasing();

	// drop gun
	bool bDropGun;
	void DropGun();

	// Put away gun
	/////////// GUN RECOIL /////////////////
	void PlayRecoil();
	FTimerHandle PlayRecoilHandle;
	FTimeline RecoilGunTimeline;
	////////////////////////////////////////////

	/////////////// Constantly Shooting ///////////
	bool bConstantlyShoot;
	FTimerHandle ConstantlyShootHandle;

	/////////////// INVENTORY ////////////////
	class UInventoryComponent* PlayerInventory;
	void AddAmmoToInventory();

	/////////////// Reloading ////////////////
	float OriginalMagazineCapacity;
	bool bIsReloading;
	FTimerHandle ReloadHandle;
	const FWeaponAnimation& ReloadAnimAccordingToSituation();
	bool CanReload();
	void RemoveAmmunitionFromInventory(FItemStruct* AmmoFromInventory);
	void Reload();
	////////////////////

	//////////// First bullet without any recoil ////////////////
	bool bFirstBulletWithoutRecoil = true;
	void ShouldFirstBulletGoStraight() { bFirstBulletWithoutRecoil = true; }
	FTimerHandle FirstBulletHandle;
	/////////////////////////////////

	////////////// RecoilCamera ///////////////
	bool bCanRecoilCamera;
	bool bShouldCameraInterpBack;
	float RandomRecoilYaw;
	float RandomRecoilPitch;
	float RandomValueForCameraYRecoil;
	FRotator InitialCameraRotation;
	FTimerHandle ShootTimerHandle;
	FTimeline RecoilCameraTimeline;
	void SetCameraRecoil();
	void ResetVariablesForCameraRecoil();
	void BackCameraToItsInitialRotation();
	void UpRecoilCameraUsingCurve(float Delta);
	void CameraInterpBackToInitialPosition(float Delta);
	////////////////////
	
	///////////// Delay Shoot /////////
	bool bShouldDelayShoot;
	void DelayShoot() { bShouldDelayShoot = false; }
	FTimerHandle DelayShootHandle;
	///////////////////////////

	///////////// zoom ///////////////
	void SetUpZoom();
	class AScope* ScopeActor;

	/////////////// ADS GUN //////////////////
	EStatusOfAimedGun StatusOfGun = HipFire;

	void PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const;

	void SetupFloatTimeline(FTimeline* TimelineToCreate, FName TimelineProgressFuncName, FName TimelineFinishedFuncName, UCurveFloat* CurveForTimeline);

	class AMarineCharacter* MarinePawn;
	class AMarinePlayerController* PC;
	class UHUDWidget* HudWidget;
};