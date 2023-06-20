// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

#include "Gun.generated.h"

UENUM()
enum StatusOfAimedGun
{
	ADS UMETA(DisplayName = "ADS"),
	HipFire UMETA(DisplayName = "Hip-Fire"),
	BackToInitialPosition UMETA(DisplayName = "Original Position"),
};

UCLASS()
class MARINERUNNER_API AGun : public AActor, public ITakeInterface
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
	virtual AActor* DropItem() override;
	virtual bool ItemLocationWhenGrabbed(float SpeedOfItem) override;

	//If Gun should be with scope then 
	// * add Scope actor blueprint class
	// * add this Event in EventGraph (Functions->Override->ZoomScope)
	// * Call Zoom event from BP_Scope in ZoomScope event
	UFUNCTION(BlueprintImplementableEvent)
		int32 ZoomScope(float WheelAxis, bool bShouldRestartScope = false);

	void WaitToReload();
	void CancelReload();
	void Shoot();
	void GunSwayWhileMoving();
	void ShootReleased();
	void SetGunSwayWhileMovingTimer(bool bShouldClear = false);

	float GetAmmoDistance() const { return AmmoDistance; }
	bool GetShouldChangeMouseSensitivityADS() const { return bShouldChangeMouseSensitivityADS; }
	bool GetIsReloading()  const { return bIsReloading; }

	int32 GetMagazineCapacity() const { return MagazineCapacity; }

	void SetBulletRotation(FRotator NewBulletRotation) { BulletRotation = NewBulletRotation; }
	void SetCanGunSwayTick(bool bCan) { bCanGunSwayTick = bCan; }
	void SetCanShoot() { bCanShoot = true; }
	void SetCanSway(bool bCan) { bCanSway = bCan; }
	void SetMarinePawn(class AMarineCharacter* NewActor) { MarinePawn = NewActor; }
	void SetStatusOfGun(StatusOfAimedGun NewStatus) { StatusOfGun = NewStatus; }

	void SetMagazineCapacity(int32 NewMagazineCapacity) { MagazineCapacity = NewMagazineCapacity; }

	void SetWeaponInHud(bool bChangeStoredAmmoText = false, bool bChangeWeaponImage = false);

	//Take And Drop
	FString GetAmmoName() const { return Ammo_Name; }
	FVector GetRelativeLocationInPawn() const { return RelativeLocationInPawn; }
	void SetItemFromInventory(struct FItemStruct* NewItemFromInventory) { ItemFromInventory = NewItemFromInventory; }
	void EquipWeapon(bool bShouldPlaySound = true, bool bIsThisCurrentGun = true);


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
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		FRotator RelativeRotationInPawn;

	//General Damage
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float Damage;
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
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammunition")
		int32 MagazineCapacity = 10;
	//When a player picks up a weapon for the first time, the value will be added to the inventory.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammunition")
		int32 StoredAmmo = 50;
	//The name of the item from the inventory that will be the ammunition for this weapon. It must be the same as the one in the inventory.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Ammunition")
		FString Ammo_Name = "ColtAmmo";

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

	// How fast ammo is moving forward. If Bullet has physics then this variable is Impulse Force
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float AmmoSpeed;
	// What distance should Ammo pass when bullet starts falling down
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float AmmoDistance;
	// How fast Ammo will falling down when AmmoDistance hit the number
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float AmmoFallingDown;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		float AmmoImpulseForce;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet")
		bool bCanBulletGoThrough;
	// How much damage should be reduced after one object in percentage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects", 
		meta = (EditCondition = "bCanBulletGoThrough", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float DamageReduceAfterObject;
	// How much impulse should be reduced after one object in percentage
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects", 
		meta = (EditCondition = "bCanBulletGoThrough", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float ImpulseReduceAfterObject;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Bullet Through Objects", meta = (EditCondition = "bCanBulletGoThrough", EditConditionHides))
		int32 MaxObjectsForBulletToGoThrough;

	//Bullet Type that will be fired from Gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Objects to Spawn")
		TSubclassOf<AActor> BulletClass;
	//Actor that will spawn on the location from Socket "BulletDrop". Its for casing that is dumped from gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Objects to Spawn")
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
		bool bShouldWaitToPlayRecoil;
	//How long should wait to play recoil timelines (camera and gun recoil)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldWaitToPlayRecoil", EditConditionHides))
		float PlayRecoilTime = 0.6f;

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
	//This animation will play when there is no bullets left and the player is shooting the last one
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		UAnimationAsset* ShootWithNoBulletsAnimation;

	//Rotation Sway Pitch
	//Maximal Pitch rotation of Gun when player looking down
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeBack = -8.f;
	//Maximal Pitch rotation of Gun when player looking up
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeUp = 8.f;
	//Speed of Gun that will reach the Range Sway Pitch
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayPitch = 3.f;
	
	//Rotation Sway Yaw
	//Maximal Yaw rotation of Gun when player looking left
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeBack = -10.f;
	//Maximal Yaw rotation of Gun when player looking right
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeUp = 10.f;
	//Speed of Gun that will reach the Range Sway Yaw
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayYaw = 3.f;

	//Location Sway X
	//Maximal X location of Gun when player moving forward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeBack = -10.f;
	//Maximal X location of Gun when player moving backward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeUp = 20.f;
	//Speed of Gun that will reach the Range Sway X
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayX = 3.f;

	//Location Sway Y
	//Maximal Y location of Gun when player moving right (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeBack = -10.f;
	//Maximal Y location of Gun when player moving left (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeUp = 20.f;
	//Speed of Gun that will reach the Range Sway Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayY = 3.f;

	//Speed of Moving Gun while Player is moving
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayWhileMoving = 350.f;
	//Multiplier of Calculeted Y by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationYSwayWhileMoving = 2.f;
	//Multiplier of Calculeted Z by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Z
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationZSwayWhileMoving = 1.2;

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
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PickUpSound;

	//RecoilTimeline
	void Playtimeline(class UTimelineComponent* TimeLineComp);
	class UTimelineComponent* SetupTimeline(class UTimelineComponent* TimeLineComp, UCurveFloat* MostImportantCurve, FName TimeLineName, FName TimeLineDirection, float TimeLineLenght, FName TimelineCallbackFunction,
		FName TimelineFinishedFunction);

	
	bool CanShoot();
	bool bCanShoot = true;
	bool bCanSway = true;
	bool bCanGunSwayTick = false;
	bool bCanDropTheGun = true;

	//////////////BULLET///////////
	FRotator BulletRotation;
	void SpawnBullet();
	void AddEffectsToShooting();
	///////////////////////////////

	///////////GUN and CAMERA RECOIL/////////////////
	void PlayRecoil();
	FTimerHandle PlayRecoilHandle;
	////////////////////////////////////////////

	///////////////Constantly Shooting///////////
	bool bConstantlyShoot;
	FTimerHandle ConstantlyShootHandle;

	//////////////TakeAndDrop//////////////////
	bool bDidTakeThisWeapon;
	bool bIsGrabbingEnded;
	bool IsGunAtTheWeaponLocation();
	struct FItemStruct* ItemFromInventory;
	AActor* ChangeToAnotherWeapon(int32 AmountOfWeapons);
	void DropTheGun();
	void AddAmmoToInventory();
	/////////////////

	/////////////////////HUD WIDGET/////////////////////H
	// If NewHudWidget is a pointer to the HudWiget from the player then Hide weapon, otherwise			//
	// check if the weapon has a HudWidget if so then Hide weapon(because this means that the player	//
	// has just dropped the weapon																		//
	void SetHudWidget(class UHUDWidget* NewHudWidget);
	//////////////////////////////////////////////////

	//////////////DropCasing//////////
	void DropCasing();

	///////////////Reloading////////////////
	float CopyOfMagazineCapacity;
	bool bIsReloading;
	FTimerHandle ReloadHandle;
	class UAudioComponent* SpawnedReloadSound;
	void Reload();
	////////////////////

	////////////First bullet without any recoil////////////////
	bool bFirstBulletWithoutRecoil = true;
	void ShouldFirstBulletGoStraight() { bFirstBulletWithoutRecoil = true; }
	FTimerHandle FirstBulletHandle;
	/////////////////////////////////

	//////////////////////OnHit////////////////////
	class UAudioComponent* SpawnedHitGroundSound;
	AActor* HitActor;

	//////////////RecoilCamera///////////////
	bool bCanRecoilCamera;
	bool bShouldInterpBack;
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
	void InterpBackToInitialPosition();
	////////////////////

	/////////////////Gun Sway////////////////
	void GunSway();
	FRotator GunRotationSway;
	FTimerHandle GunSwayWhileMovingHandle;
	/////////////////////////////////
	
	/////////////Delay Shoot/////////
	bool bShouldDelayShoot;
	void DelayShoot() { bShouldDelayShoot = false; }
	FTimerHandle DelayShootHandle;
	///////////////////////////

	///////////////ADS GUN//////////////////
	void AimTheGun(float Delta);
	StatusOfAimedGun StatusOfGun = HipFire;
	///////////////////////////

	class AMarineCharacter* MarinePawn;
	class AMarinePlayerController* PC;
	class UHUDWidget* HudWidget;
};