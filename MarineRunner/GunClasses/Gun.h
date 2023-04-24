// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"

#include "Gun.generated.h"

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

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* BaseSkeletalMesh;

	FVector GetRelativeLocationInPawn() const { return RelativeLocationInPawn; }
	FRotator GetRelativeRotationInPawn() const { return RelativeRotationInPawn; }

	USkeletalMeshComponent* GetBaseSkeletalMesh() const { return BaseSkeletalMesh; }

	bool GetIsAutomatic() const { return bIsAutomatic; }
	void ShouldConstantlyShoot(bool bShould) { bConstantlyShoot = bShould; }
	void Reload();

	void Shoot();
	void GunSwayWhileMoving();
	void ShootReleased();

	float GetAmmoDistance() const { return AmmoDistance; }
	//bool GetCanSway() { return bCanSway; }
	bool GetCanDropGun() { return bCanDropTheGun; }

	void SetBulletRotation(FRotator NewBulletRotation) { BulletRotation = NewBulletRotation; }
	void SetCanAimTheGun(int CanAim) { CanAimTheGun = CanAim; }
	void SetCanGunSwayTick(bool bCan) { bCanGunSwayTick = bCan; }
	void SetCanSway(bool bCan) { bCanSway = bCan; }
	void SetMarinePawn(AActor* NewActor) { MarinePawn = NewActor; }
	void SetHudWidget(class UHUDWidget* NewHudWidget);

	void SetWeaponInHud(bool bChangeStoredAmmoText = false, bool bChangeWeaponImage = false);

private:
	UFUNCTION()
		void RecoilAnimTimelineCallback(float x);

	UFUNCTION()
		void RecoilAnimTimelineFinishedCallback();
	UFUNCTION()
		void RecoilCameraTimelineCallback(float x);

	UFUNCTION()
		void RecoilCameraTimelineFinishedCallback();

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		FVector RelativeLocationInPawn;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		FRotator RelativeRotationInPawn;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Aiming")
		FVector RelativeLocationInPawnWhileAiming;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Aiming")
		float SpeedOfAiming = 14.f;
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Aiming")
		float SpeedOfAimingBack = 7.f;
	//This number will be subdivide with value from Recoil
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun|Aiming")
		float DividerOfRecoilWhileAiming = 3.5f;

	//General Damage
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float Damage;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		bool bIsAutomatic;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 MagazineCapacity = 10;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 StoredAmmo = 50;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		UTexture2D* GunHUDTexture;


	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	//Use curve for Camera Recoil or use random Pitch and Yaw numbers for camera Recoil
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil")
		bool bShouldUseCurveRecoil;
	//Speed of Interp, after shooting there camera will be back to position before shoting and this is the speed of it
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil")
		float InitalCameraPositionSpeed = 5.f;
	//Distance that Camera is going to rotate in pitch when shooting full magazine. 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta=(EditCondition = "bShouldUseCurveRecoil"))
		float DistanceFromStart = 40.f;
	//Curve that is responisble for Yaw Camera Recoil (left, right)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil"))
		UCurveFloat* RecoilCameraCurveY;
	//How long timeline need to least. It has to be the same like is in RecoilCameraCurveY
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil", meta = (EditCondition = "bShouldUseCurveRecoil"))
		float RecoilCameraTimelineLength = 3.2f;
	//Range for random Pitch number. 0 index has to be MinNumber and 1 index has to be MaxNumber
	//0 and 1 is only for positive side (UP)
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil"))
		TArray<float>PitchRecoilRangeArray = { 20, 10};
	//Range for random Yaw number. 0 index has to be MinNumber and 1 index has to be MaxNumber.
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Random Pitch and Yaw recoil", meta = (EditCondition = "!bShouldUseCurveRecoil"))
		TArray<float>YawRecoilRangeArray = {18, 10};
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Bullet")
		TArray<float>PitchBulletRecoilArray = {-5, 5};
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Recoil|Bullet")
		TArray<float>YawBulletRecoilArray = { -5, 5 };;

	//How fast ammo is moving forward. If Bullet has physics then this variable is Impulse Force
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoSpeed;
	//What distance should Ammo pass when bullet starts falling down
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoDistance;
	//How fast Ammo will falling down when AmmoDistance hit the number
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoFallingDown;
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoImpulseForce;

	//Bullet Type that will be fired from Gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TSubclassOf<AActor> BulletClass;

	//Actor that will spawn on the location from Socket "BulletDrop". Its for casing that is dumped from gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TSubclassOf<AActor> DropBulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Animation|Animations for Gun from FBX")
		UAnimationAsset* ShootAnimation;

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
		class USoundBase* ShootingSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		class USoundBase* EmptyMagazineSound;

	//RecoilTimeline
	void Playtimeline(class UTimelineComponent* TimeLineComp);
	class UTimelineComponent* SetupTimeline(class UTimelineComponent* TimeLineComp, UCurveFloat* MostImportantCurve, FName TimeLineName, FName TimeLineDirection, float TimeLineLenght, FName TimelineCallbackFunction,
		FName TimelineFinishedFunction);

	FRotator BulletRotation;

	bool bCanShoot = true;
	bool bCanSway = true;
	bool bCanGunSwayTick = false;
	bool bCanDropTheGun = true;

	//Constanly Shooting
	bool bConstantlyShoot;
	FTimerHandle ConstantlyShootHandle;

	//Reloading
	float CopyOfMagazineCapacity;

	//RecoilCamera
	bool bCanRecoilCamera;
	FRotator InitialCameraRotation;
	FTimerHandle FixingBugHandle;
	float RandomRecoilYaw;
	float RandomRecoilPitch;
	
	float TimeRecoilCameraElapsed;
	void UpRecoilCamera(float Delta);
	void InterpBackToInitialPosition(float Delta);
	bool bShouldInterpBack;

	void GunSway(float Delta);
	FRotator GunRotationSway;

	void AimTheGun(float Delta);
	int32 CanAimTheGun; //0 - cant, 1 - can, 2 - back to original position

	AActor* MarinePawn;
	class AMarinePlayerController* PC;
	class UHUDWidget* HudWidget;

};
