// Fill out your copyright notice in the Description page of Project Settings.

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

	UFUNCTION(BlueprintPure)
		class AMarineCharacter* GetMarinePawn() const { return MarinePawn; }

	void TakeGun(class AMarineCharacter* Player);
	void DrawGun();
	void PutAwayGun();
	void HideGun();

	//If Gun should be with scope then 
	// * add Scope actor blueprint class
	// * add this Event in EventGraph (Functions->Override->ZoomScope)
	// * Call Zoom event from BP_Scope in ZoomScope event
	UFUNCTION(BlueprintImplementableEvent)
		int32 ZoomScope(float WheelAxis, bool bShouldRestartScope = false);
	UFUNCTION(BlueprintImplementableEvent)
		int32 ActivateZoom(bool bShouldActivate = false);
	UFUNCTION(BlueprintImplementableEvent)
		void ChangeScopeResolution(EStatusOfAimedGun CurrentStatusOfGun);

	void WaitToReload();
	void CancelReload();

	void Shoot();
	void ShootReleased();

	float GetAmmoDistance() const { return BulletData.Distance; }
	bool GetShouldChangeMouseSensitivityADS() const { return bShouldChangeMouseSensitivityADS; }
	bool GetIsReloading()  const { return bIsReloading; }

	int32 GetMagazineCapacity() const { return MagazineCapacity; }

	void SetBulletRotation(FRotator NewBulletRotation) { BulletRotation = NewBulletRotation; }

	void SetMarinePawn(class AMarineCharacter* NewActor) { MarinePawn = NewActor; }

	void AimTheGun(EStatusOfAimedGun NewGundStatus);

	void SetMagazineCapacity(int32 NewMagazineCapacity) { MagazineCapacity = NewMagazineCapacity; }

	void UpdateWeaponDataInHud(bool bChangeStoredAmmoText = false, bool bChangeWeaponImage = false);

	//Take And Drop
	FString GetAmmoName() const { return SpawnedAmmoItemData.Item_Name; }
	void SetItemFromInventory(struct FItemStruct* NewItemFromInventory) { AmmunitionFromInventory = NewItemFromInventory; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* GunSkeletalMesh;

	UFUNCTION()
		void ShootRecoilTimelineProgress(float x);

	UFUNCTION()
		void ShootRecoilTimelineFinished();
	UFUNCTION()
		void RecoilCameraTimelineCallback(float x);

	UFUNCTION()
		void RecoilCameraTimelineFinishedCallback() {};

	//General Damage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		FBulletStruct BulletData;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float ReloadTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bReloadOneBullet;
	//Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bManyBulletAtOnce;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		int32 HowManyBulletsToSpawn = 10;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Ammunition")
		int32 MagazineCapacity = 10;
	//When a player picks up a weapon for the first time, the value will be added to the inventory.
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Ammunition")
		int32 StoredAmmo = 50;
	//The item that will be the ammunition for this weapon.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammunition")
		TSubclassOf<class APickupItem> AmmunitionItemClass;

	// If True then ammunition on UI will be below the Gun icon picture and its 460x260
	// Useful for longer weapons
	// If False then ammunition on UI will be on the left side of the gun icon picture and its 260x150
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|UI")
		bool bAmmoCounterBelowGunHUD;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|UI")
		UTexture2D* GunHUDTexture;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		bool bShouldChangeMouseSensitivityADS = false;
	//This number will be subdivided with value from Recoil (camera, gun recoil)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfRecoilWhileADS = 3.5f;
	//This number will be subdivided with value from Recoil (bullet)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfBulletRecoilWhileADS = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|DelayShoot")
		float DelayShootTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection")
		bool bCasingEjectionWhileReloading;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection")
		bool bShouldRandomizeRotationOfCasing = true;
	//Actor that will spawn on the location from Socket "BulletDrop". Its for casing that is dumped from gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection")
		TSubclassOf<AActor> DropBulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	//Use curve for Camera Recoil or use random Pitch and Yaw numbers for camera Recoil
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil")
		bool bShouldUseCurveRecoil;
	//Speed of Interp, after shooting there camera will be back to position before shoting and this is the speed of it
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float InitalCameraPositionSpeed = 5.f;
	//Distance that Camera is going to rotate in pitch when shooting full magazine. 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float DistanceFromStart = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float FirstBulletWithoutRecoilTime = 1.f;
	//How long timeline need to least. It has to be the same like is in RecoilCameraCurveY
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		float RecoilCameraTimelineLength = 3.2f;
	//Curve that is responisble for Yaw Camera Recoil (left, right)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		UCurveFloat* RecoilCameraCurveY;
	//Takes Value from RecoilCameraCurveY and randomize it with value from TArray<>. First Number in TArray is The Min and Second is The Max.
	//If TArray<> has more then 2 values then Value will not be randomize
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil", EditConditionHides))
		TArray<float> RandomRangeFromRecoilCurveY = {-0.2f, 0.2f};
	//Should wait to execute Recoil timelines. When you want to play a little bit of animation and then add recoil for the camera and the gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil")
		bool bShouldWaitToPlayRecoil = true;
	//How long should wait to play recoil timelines (camera and gun recoil)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldWaitToPlayRecoil", EditConditionHides))
		float PlayRecoilTime = 0.06f;

	//The curve responsible for how quickly the screen reaches PitchRecoilRandomNumber and YawRecoilRandomNumber and returns to its rotation. 
	//It was added for smoothness. It should have a length like the one in Animation|Setting Up Animation Recoil in RecoilAnimTimelineLength 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		UCurveFloat* RecoilCameraCurveRandomRotation;
	//Range for random Pitch number. 0 index has to be MinNumber and 1 index has to be MaxNumber
	//0 and 1 is only for positive side (UP)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		TArray<float>PitchRecoilRangeArray = { 20, 10};
	//Range for random Yaw number. 0 index has to be MinNumber and 1 index has to be MaxNumber.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil", EditConditionHides))
		TArray<float>YawRecoilRangeArray = {18, 10};
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Bullet")
		TArray<float>PitchBulletRecoilArray = {-5, 5};
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Bullet")
		TArray<float>YawBulletRecoilArray = { -5, 5 };;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponFirstTimeTakeAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponShootAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponReloadAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponDrawAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponHideAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponADSInAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponADSOutAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		FWeaponAnimation WeaponShootWithNoBulletsAnim;

	//Recoil when player shot
	
	//Max length of RecoilTimeline, The curves need to have the same length as Timeline.
	//This is also responsible for Rate of fire. (Player can shoot only when Timeline ends)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		float RecoilAnimTimelineLength = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		float MultiplierOfRecoilCurvePitch = 8;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		UCurveFloat* ShootFOVCurve;

	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ShootingSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds", meta = (EditCondition = "bPlayShootAnimationAfterFire", EditConditionHides))
		USoundBase* AfterShootSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* EmptyMagazineSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ReloadSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* DrawGunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PutAwayGunSound;

	float OriginalPlayerFOV = 90.f;
	
	////////////// SHOOT ////////////
	bool bCanShoot = true;
	bool CanShoot();
	void SetCanShoot();
	////////////////////////////////

	////////////// BULLET ///////////
	FRotator BulletRotation;
	void RandomBulletDirection(FRotator& NewBulletRotation);
	void SpawnBullet();
	///////////////////////////////

	///////////// GUN EFFECTS ///////
	void PlayGunShootAnimation();
	void AddEffectsToShooting();
	void DropCasing();

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
	FItemStruct SpawnedAmmoItemData;
	FItemStruct* AmmunitionFromInventory;
	void AddAmmoToInventory();
	void SpawnAmmunitionObjectForVariables();
	bool GetPointerToAmmoFromInventory();

	/////////////// Reloading ////////////////
	float OriginalMagazineCapacity;
	bool bIsReloading;
	FTimerHandle ReloadHandle;
	class UAudioComponent* SpawnedReloadSound;
	bool CanReload();
	void ReloadEffects();
	void RemoveAmmunitionFromInventory();
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
	float TimeRecoilCameraElapsed;
	float RandomValueForCameraYRecoil;
	FRotator InitialCameraRotation;
	FTimerHandle ShootTimerHandle;
	FTimeline RecoilCameraTimeline;
	void SetCameraRecoil();
	void ResetVariablesForCameraRecoil();
	void BackCameraToItsInitialRotation();
	void UpRecoilCamera(float Delta);
	void CameraInterpBackToInitialPosition(float Delta);
	////////////////////
	
	///////////// Delay Shoot /////////
	bool bShouldDelayShoot;
	void DelayShoot() { bShouldDelayShoot = false; }
	FTimerHandle DelayShootHandle;
	///////////////////////////

	/////////////// ADS GUN //////////////////
	EStatusOfAimedGun StatusOfGun = HipFire;
	///////////////////////////

	void SetupFloatTimeline(FTimeline* TimelineToCreate, FName TimelineProgressFuncName, FName TimelineFinishedFuncName);

	class AMarineCharacter* MarinePawn;
	class AMarinePlayerController* PC;
	class UHUDWidget* HudWidget;
};