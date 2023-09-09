// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "MarineRunner/Inventory/PickupItem.h"
#include "MarineRunner/Inventory/InventoryComponent.h"
#include "BulletData.h"

#include "Gun.generated.h"

UENUM()
enum StatusOfAimedGun
{
	ADS UMETA(DisplayName = "ADS"),
	HipFire UMETA(DisplayName = "Hip-Fire"),
	BackToInitialPosition UMETA(DisplayName = "Original Position"),
};

UCLASS()
class MARINERUNNER_API AGun : public APickupItem
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

	//Take
	virtual void TakeItem(class AMarineCharacter* MarineCharacter, bool& bIsItWeapon) override;
	virtual void ItemHover(class UHUDWidget* MarineHUDWidget) override;
	virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) override;
	virtual AActor* DropItem() override;
	virtual bool MoveItemToEquipPosition(float SpeedOfItem) override;

	//If Gun should be with scope then 
	// * add Scope actor blueprint class
	// * add this Event in EventGraph (Functions->Override->ZoomScope)
	// * Call Zoom event from BP_Scope in ZoomScope event
	UFUNCTION(BlueprintImplementableEvent)
		int32 ZoomScope(float WheelAxis, bool bShouldRestartScope = false);
	UFUNCTION(BlueprintImplementableEvent)
		int32 ActivateZoom(bool bShouldActivate = false);

	void WaitToReload();
	void CancelReload();

	void Shoot();
	void ShootReleased();

	void GunSwayWhileMoving();
	void SetGunSwayWhileMovingTimer(bool bShouldClear = false);

	float GetAmmoDistance() const { return BulletData.Distance; }
	bool GetShouldChangeMouseSensitivityADS() const { return bShouldChangeMouseSensitivityADS; }
	bool GetIsReloading()  const { return bIsReloading; }

	int32 GetMagazineCapacity() const { return MagazineCapacity; }

	void SetBulletRotation(FRotator NewBulletRotation) { BulletRotation = NewBulletRotation; }

	void SetCanGunSwayTick(bool bCan) { bActivateGunSway = bCan; }
	void SetCanSway(bool bCan) { bCanSway = bCan; }

	void SetMarinePawn(class AMarineCharacter* NewActor) { MarinePawn = NewActor; }
	void SetStatusOfGun(StatusOfAimedGun NewStatus) { StatusOfGun = NewStatus; }

	void SetMagazineCapacity(int32 NewMagazineCapacity) { MagazineCapacity = NewMagazineCapacity; }

	void UpdateWeaponDataInHud(bool bChangeStoredAmmoText = false, bool bChangeWeaponImage = false);

	//Take And Drop
	FString GetAmmoName() const { return SpawnedAmmoItemData.Item_Name; }
	FVector GetRelativeLocationInPawn() const { return RelativeLocationInPawn; }
	bool GetIsGrabbingEnded() const { return bEquipPositionMoveCompleted; }
	void SetItemFromInventory(struct FItemStruct* NewItemFromInventory) { AmmunitionFromInventory = NewItemFromInventory; }
	void EquipWeapon(bool bIsThisCurrentGun = true);

	// Albertos
	virtual void ChangeSimulatingPhysics(bool bChange = true) override;
	virtual void SetDissolveMaterial(UMaterialInstance* NewMaterial, USkeletalMeshComponent* SkeletalMesh = nullptr) override;
	virtual void SetCollisionNewResponse(ECollisionChannel ChannelName, ECollisionResponse NewResponse) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* BaseSkeletalMesh;

	UFUNCTION()
		void RecoilAnimTimelineCallback(float x);

	UFUNCTION()
		void RecoilAnimTimelineFinishedCallback();
	UFUNCTION()
		void RecoilCameraTimelineCallback(float x);

	UFUNCTION()
		void RecoilCameraTimelineFinishedCallback() {};

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		FVector RelativeLocationInPawn;

	//General Damage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		FBulletStruct BulletData;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float ReloadTime = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bIsAutomatic;
	//should play the shot animation after the shot. If it is equal to false, the shot animation will be played along with the shot
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bPlayShootAnimationAfterFire = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bReloadOneBullet;
	//Should spread bullets from barrel like from shotgun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bManyBulletAtOnce;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun", meta = (EditCondition = "bManyBulletAtOnce", EditConditionHides))
		int32 HowManyBulletsToSpawn = 10;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float DropImpulseDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		UCurveFloat* ShootFOVCurve;

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
		FVector RelativeLocationInPawnWhileADS;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		bool bShouldChangeMouseSensitivityADS = false;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float SpeedOfInterpADS = 14.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float SpeedOfBackToHipFire = 7.f;
	//This number will be subdivided with value from Recoil (camera, gun recoil)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfRecoilWhileADS = 3.5f;
	//This number will be subdivided with value from Recoil (bullet)
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfBulletRecoilWhileADS = 3.f;
	//This number will be subdivided with value from responsible for Gun Sway
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfGunSwayADS = 1.5f;
	//This number will be subdivided with value from responsible for Gun Sway while moving
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|ADS")
		float DividerOfGunSwayMovingADS = 2.f;

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
		UAnimationAsset* ShootAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		UAnimationAsset* ReloadAnimation;
	//This animation will play when there is no bullets left and the player is shooting the last one
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		UAnimationAsset* ShootWithNoBulletsAnimation;

	//Rotation Sway Pitch
	//Maximal Pitch rotation of Gun when player looking down
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeBack = -7.f;
	//Maximal Pitch rotation of Gun when player looking up
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeUp = 7.f;
	//Speed of Gun that will reach the Range Sway Pitch
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayPitch = 7.f;
	
	//Rotation Sway Yaw
	//Maximal Yaw rotation of Gun when player looking left
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeBack = -5.5f;
	//Maximal Yaw rotation of Gun when player looking right
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeUp = 5.5f;
	//Speed of Gun that will reach the Range Sway Yaw
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayYaw = 3.f;

	//Location Sway X
	//Maximal X location of Gun when player moving forward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeBack = -4.f;
	//Maximal X location of Gun when player moving backward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeUp = 5.f;
	//Speed of Gun that will reach the Range Sway X
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayX = 7.f;

	//Location Sway Y
	//Maximal Y location of Gun when player moving right (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeBack = -4.f;
	//Maximal Y location of Gun when player moving left (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeUp = 4.f;
	//Speed of Gun that will reach the Range Sway Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayY = 4.f;

	//Speed of Moving Gun while Player is moving
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayWhileMoving = 550.f;
	//Multiplier of Calculeted Y by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationYSwayWhileMoving = 0.8f;
	//Multiplier of Calculeted Z by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Z
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationZSwayWhileMoving = 0.4f;

	//Recoil when player shot
	
	//Max length of RecoilTimeline, The curves need to have the same length as Timeline.
	//This is also responsible for Rate of fire. (Player can shoot only when Timeline ends)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		float RecoilAnimTimelineLength = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		float MultiplierOfRecoilCurvePitch = 8;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		UCurveFloat* RecoilAnimCurveScale;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		UCurveFloat* RecoilAnimCurvePitch;
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Animation Recoil")
		UCurveFloat* RecoilAnimCurveLocationX;
	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
		class UTimelineComponent* RecoilAnimTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Timelines")
		class UTimelineComponent* RecoilCameraTimeline;
	UPROPERTY()
		TEnumAsByte<ETimelineDirection::Type> RecoilAnimTimelineDirection;

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
		USoundBase* HitGroundSound;

	//RecoilTimeline
	void Playtimeline(class UTimelineComponent* TimeLineComp);
	class UTimelineComponent* SetupTimeline(class UTimelineComponent* TimeLineComp, UCurveFloat* MostImportantCurve, FName TimeLineName, FName TimeLineDirection, float TimeLineLenght, FName TimelineCallbackFunction,
		FName TimelineFinishedFunction);

	float CopyOfFOV = 90.f;
	
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
	void PlayGunHitObjectSound(AActor* OtherActor, const FHitResult& Hit);

	/////////// GUN and CAMERA RECOIL /////////////////
	void PlayRecoil();
	FTimerHandle PlayRecoilHandle;
	////////////////////////////////////////////

	/////////////// Constantly Shooting ///////////
	bool bConstantlyShoot;
	FTimerHandle ConstantlyShootHandle;

	/////////////// INVENTORY ////////////////
	FItemStruct SpawnedAmmoItemData;
	FItemStruct* AmmunitionFromInventory;
	void RemoveGunFromAlbertosInventory();
	void AddAmmoToInventory();
	void SpawnAmmunitionObjectForVariables();
	bool GetPointerToAmmoFromInventory();

	////////////// TakeAndDrop //////////////////
	bool bDidTakeThisWeapon;
	bool bEquipPositionMoveCompleted;
	bool IsGunAtTheEquipLocation();
	bool bCanDropTheGun = true;
	AActor* EquipAnotherWeapon(int32 AmountOfWeapons);
	void DropTheGun();
	void DisableTheGun();
	/////////////////

	///////////////////// HUD WIDGET /////////////////////H
	// If NewHudWidget is a pointer to the HudWiget from the player then Hide weapon, otherwise			//
	// check if the weapon has a HudWidget if so then Hide weapon(because this means that the player	//
	// has just dropped the weapon																		//
	void SetHudWidget(class UHUDWidget* NewHudWidget);
	//////////////////////////////////////////////////

	/////////////// Reloading ////////////////
	float CopyOfMagazineCapacity;
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

	////////////////////// OnHit ////////////////////
	class UAudioComponent* SpawnedHitGroundSound;
	AActor* HitActor;

	////////////// RecoilCamera ///////////////
	bool bCanRecoilCamera;
	bool bShouldCameraInterpBack;
	float RandomRecoilYaw;
	float RandomRecoilPitch;
	float TimeRecoilCameraElapsed;
	float RandomValueForCameraYRecoil;
	FRotator InitialCameraRotation;
	FTimerHandle ShootTimerHandle;
	void SetCameraRecoil();
	void ResetVariablesForCameraRecoil();
	void BackCameraToItsInitialRotation();
	void UpRecoilCamera();
	void CameraInterpBackToInitialPosition();
	////////////////////

	///////////////// Gun Sway ////////////////
	bool bCanSway = true;
	bool bActivateGunSway = false;
	bool GunADSSway(float LookUpValue, float LookRightValue);
	bool CalculateGunSway(FVector & CalculatedLocation, FRotator &CalculatedRotation, float Delta);
		//Using Lemniscate Of Bernoulli to sway gun while moving 
	FVector CalculateLOBGunSwayWhileMoving();
	void GunSway();
	FRotator GunRotationSway;
	FTimerHandle GunSwayWhileMovingHandle;
	/////////////////////////////////
	
	///////////// Delay Shoot /////////
	bool bShouldDelayShoot;
	void DelayShoot() { bShouldDelayShoot = false; }
	FTimerHandle DelayShootHandle;
	///////////////////////////

	/////////////// ADS GUN //////////////////
	void AimTheGun(float Delta);
	StatusOfAimedGun StatusOfGun = HipFire;
	///////////////////////////

	class AMarineCharacter* MarinePawn;
	class AMarinePlayerController* PC;
	class UHUDWidget* HudWidget;
};