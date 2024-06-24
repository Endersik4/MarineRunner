// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/DamageInterface.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "MarinePlayer.generated.h"

/// <summary>
/// Add Physics material to player so the player cant bounce too much on obstacles
/// </summary>
UCLASS()
class MARINERUNNER_API AMarineCharacter : public APawn, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMarineCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, const EWeaponType& WeaponType, float NewSphereRadius = 0.f) override; //C++ ONLY

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ChangeMouseSensitivity(const FSettingSavedInJsonFile& NewMouseSensitivity, bool bResetMouseSensitivity = false);
	void LoadFieldOfViewFromSettings();

	bool GetIsWallrunning() const;
	bool GetIsPlayerMovingToHookLocation() const;
	bool GetIsCrouching() const;
	bool GetIsInSlowMotion() const;
	bool GetIsPullingUp() const;
	bool GetIsInAir() const;
	FVector GetCameraLocation() const;
	bool GetIsInPauseMenu() const;

	FORCEINLINE float GetMovementForce() const { return MovementSpeed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE bool GetIsInCutscene() const { return bIsInCutscene; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetShouldPlayerGoForward() const { return bConstantlyGoForward; }
	FORCEINLINE TObjectPtr<class UCapsuleComponent> GetPlayerCapsule() const { return CapsulePawn; }
	FORCEINLINE TObjectPtr<class USceneComponent> GetGroundLocationSceneComponent() const { return GroundLocationSceneComponent; }
	FORCEINLINE TObjectPtr<class USceneComponent> GetRoofLocationSceneComponent() const { return RoofLocationSceneComponent; }
	FORCEINLINE TObjectPtr<class USkeletalMeshComponent> GetArmsSkeletalMesh() const { return ArmsSkeletalMesh; }
	FORCEINLINE TObjectPtr<class UHUDWidget> GetHudWidget() const { return HudWidget; }
	FORCEINLINE TObjectPtr<class UWeaponInventoryComponent> GetWeaponInventoryComponent() const { return WeaponInventoryComponent; }
	FORCEINLINE TObjectPtr<class UInventoryComponent> GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE TObjectPtr<class UCameraComponent> GetCamera() const { return Camera; }
	FORCEINLINE TObjectPtr<class UPauseMenuComponent> GetPauseMenuComponent()const { return PauseMenuComponent; }
	FORCEINLINE TObjectPtr<class UDashComponent> GetDashComponent() const { return DashComponent; }
	FORCEINLINE TObjectPtr<class UWeaponHandlerComponent> GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }
	FORCEINLINE TObjectPtr<class UJumpComponent> GetJumpComponent() const { return JumpComponent; }
	FORCEINLINE TObjectPtr<class UPullUpComponent> GetPullUpComponent() const { return PullUpComponent; }
	FORCEINLINE TObjectPtr<class UMessageHandlerComponent> GetMessageHandlerComponent() const { return MessageHandlerComponent; }
	FORCEINLINE TObjectPtr<class USaveLoadPlayerComponent> GetSaveLoadPlayerComponent() const { return SaveLoadPlayerComponent; }
	FORCEINLINE TObjectPtr<class UArmsSwayComponent> GetArmsSwayComponent() const { return ArmsSwayComponent; }
	FORCEINLINE TObjectPtr<class UWallrunComponent> GetWallrunComponent() const { return WallrunComponent; }
	FORCEINLINE TObjectPtr<class UCroachAndSlide> GetCrouchAndSlideComponent() const { return CroachAndSlideComponent; }
	FORCEINLINE TObjectPtr<class USlowMotionComponent> GetSlowMotionComponent() const { return SlowMotionComponent; }
	FORCEINLINE FSettingSavedInJsonFile& GetMouseSensitivityJSON() { return OriginalMouseSensitivityJSON; }
	FORCEINLINE TObjectPtr<class AAlbertosPawn> GetAlbertosPawn() const { return AlbertoPawn; }

	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE void SetIsInCutscene(bool bNewIsInCutscene) { bIsInCutscene = bNewIsInCutscene; }
	FORCEINLINE void SetMovementForce(float NewForce) { MovementSpeed = NewForce; }
	FORCEINLINE void SetMovementForceDividerWhenInADS(float NewForceDivider) { MovementForceDividerWhenInADS = NewForceDivider; }
	FORCEINLINE void SetShouldPlayerGoForward(bool bShould) { bConstantlyGoForward = bShould; }
	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertos) { AlbertoPawn = NewAlbertos; }

	void UpdateHudWidget();
	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);
	void MakeCrosshire(bool bShouldRemoveFromParent = false);
	void ShowHUD(bool bShow = true);

	UPROPERTY(Transient)
	bool bIsPlayerInElevator = false;

	void ReplaceRootComponentRotation();

	UPROPERTY(Transient)
	int32 CheckpointNumber = 0;//ztgk, delete later
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UCapsuleComponent> CapsulePawn = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UCameraComponent> Camera = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> ArmsSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr < USceneComponent> GroundLocationSceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr < USceneComponent> RoofLocationSceneComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UWidgetInteractionComponent> WidgetInteractionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UTakeAndDrop> TakeAndDropComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UInventoryComponent> InventoryComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UJumpComponent> JumpComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UCroachAndSlide> CroachAndSlideComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UDashComponent> DashComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UWallrunComponent> WallrunComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class USlowMotionComponent> SlowMotionComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UPullUpComponent> PullUpComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UWeaponInventoryComponent> WeaponInventoryComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UPauseMenuComponent> PauseMenuComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class USwingComponent> SwingComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UWeaponHandlerComponent> WeaponHandlerComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class USpawnDeathWidgetComponent> SpawnDeathWidgetComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UMessageHandlerComponent> MessageHandlerComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class USaveLoadPlayerComponent> SaveLoadPlayerComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UArmsSwayComponent> ArmsSwayComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
	TObjectPtr<class UQuickAttackComponent> QuickAttackComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	float OriginalHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	float DividerForRadialDamage = 2.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn")
	TObjectPtr < USoundBase> MarineHitSound = nullptr;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	TObjectPtr<class AAlbertosPawn> AlbertoPawn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
	FName FirstAidKitRowName = "FirstAidKit";
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
	float FirstAidKitHealth = 35.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
	float DelayAfterUseFirstAidKit = 1.1f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
	int32 MaxAmountOfFirstAidKitsOnHud = 99;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|First Aid Kit")
	TObjectPtr < USoundBase> UseFirstAidKitSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	FSettingSavedInJsonFile OriginalMouseSensitivityJSON = FSettingSavedInJsonFile("MouseSensitivity", 0.7f);
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
	FSettingSavedInJsonFile FieldOfViewJSON = FSettingSavedInJsonFile("FieldOfView", 90.f);

	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement")
	float MovementSpeed = 6994230.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement")
	float CounterMovementForce = 2623.f;
	// When player wants to move forward and right then the forward speed is added to right speed and the player moves faster, this divider prevents this
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Movement")
	float ForwardAndRightAtTheSameTimeDivider = 1.3f;
	// if Velocity is higher then MaxVelocityForStaticCounterMovement, The higher the velocity, the lower the force counteracting the movement (when in air)
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Movement")
	float MaxVelocityForStaticCounterMovement = 1250.f;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Widgets")
	TSubclassOf<class UCrosshairWidget> CrosshairWidgetClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Widgets")
	TSubclassOf<class UHUDWidget> HUDClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement|Cutscene")
	float MovementSpeedInCutscene = 2622836.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement|Cutscene")
	float TimeBetweenNextStepInCutscene = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	FFloatRange VelocityRangeToDisableFootsteps = FFloatRange(0.f, 150.f);
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	float NormalTimeBetweenNextStep = 0.31f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	float CrouchTimeBetweenNextStep = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps")
	float WallrunTimeBetweenNextStep = 0.17f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|AI")
	float FootstepsSoundLoudnessForEnemy = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|AI")
	float FootstepsSoundMaxRangeForEnemy = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|Sounds")
	TObjectPtr<USoundBase> FootstepsSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|Sounds")
	TObjectPtr<USoundBase> FootstepsWallrunSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|Sounds")
	TObjectPtr<USoundBase> FootstepsCrouchSound = nullptr;

	UPROPERTY(Transient)
	bool bIsDead = false;
	void PlayerDead();

	UPROPERTY(Transient)
	bool bIsInCutscene = false;

	//Movement
	void Forward(float Axis);
	void Right(float Axis);
	void Move(FVector Direction, float Axis, const FName InputAxisName);
	FVector CalculateCounterMovement();
	UPROPERTY(Transient)
	float MovementForceDividerWhenInADS = 1.f;
	const float DegreeForForwardVector = -90.f;

	//If true then in the the player will always walk forward
	UPROPERTY(Transient)
	bool bConstantlyGoForward = false;

	//Footsteps sounds
	UPROPERTY(Transient)
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	bool CheckIfCanPlayFootstepsSound();
	void PlayFootstepsSound();
	FORCEINLINE void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	void CallAlbertosPressed();
	UPROPERTY(Transient)
	TObjectPtr<class UCraftingAlbertosWidget> CraftingWidget = nullptr;

	void TakePressed();
	void TakeReleased();

	//FirstAidKit
	UPROPERTY(Transient)
	bool bCanUseFirstAidKit = true;
	FTimerHandle UseFirstAidKitHandle;
	FORCEINLINE void CanUseFirstAidKit() { bCanUseFirstAidKit = true; }
	void UseFirstAidKit();


	//Widgets
	void MakeHudWidget();
	UPROPERTY(Transient)
	TObjectPtr<class UCrosshairWidget> SpawnedCrosshairWidget = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class UHUDWidget> HudWidget = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AMarinePlayerController> MarinePlayerController = nullptr;
};