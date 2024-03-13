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
class MARINERUNNER_API AMarineCharacter : public APawn, public IInteractInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMarineCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius = 0.f) override; //C++ ONLY

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
	bool GetIsMessageDisplayed() const;
	bool GetIsInPauseMenu() const;

	FORCEINLINE float GetMovementForce() const { return MovementSpeed; }
	FORCEINLINE float GetHealth() const { return Health; }
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
	FORCEINLINE FSettingSavedInJsonFile& GetMouseSensitivityJSON()  { return MouseSensitivityJSON; }
	FORCEINLINE TObjectPtr<class AAlbertosPawn> GetAlbertosPawn() const { return AlbertoPawn; }

	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE void SetMovementForce(float NewForce) { MovementSpeed = NewForce; }
	FORCEINLINE void SetMovementForceDividerWhenInADS(float NewForceDivider) { MovementForceDividerWhenInADS = NewForceDivider; }
	FORCEINLINE void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMutliplier = NewSpeed; }
	FORCEINLINE void SetShouldPlayerGoForward(bool bShould) { bConstantlyGoForward = bShould; }
	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertos) { AlbertoPawn = NewAlbertos; }

	void UpdateHudWidget();
	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);
	void MakeCrosshire(bool bShouldRemoveFromParent = false);

	bool bIsPlayerInElevator = false;

	void ReplaceRootComponentRotation();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UCapsuleComponent> CapsulePawn;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UCameraComponent> Camera;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USkeletalMeshComponent> ArmsSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr < USceneComponent> GroundLocationSceneComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr < USceneComponent> RoofLocationSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UWidgetInteractionComponent> WidgetInteractionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UTakeAndDrop> TakeAndDropComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UInventoryComponent> InventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UJumpComponent> JumpComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UCroachAndSlide> CroachAndSlideComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UDashComponent> DashComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UWallrunComponent> WallrunComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class USlowMotionComponent> SlowMotionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UPullUpComponent> PullUpComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UWeaponInventoryComponent> WeaponInventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UPauseMenuComponent> PauseMenuComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class USwingComponent> SwingComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UWeaponHandlerComponent> WeaponHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class USpawnDeathWidgetComponent> SpawnDeathWidgetComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UMessageHandlerComponent> MessageHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class USaveLoadPlayerComponent> SaveLoadPlayerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		TObjectPtr<class UArmsSwayComponent> ArmsSwayComponent;


	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		float OriginalHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		float DividerForRadialDamage = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn")
		TObjectPtr < USoundBase> MarineHitSound;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		TObjectPtr<class AAlbertosPawn> AlbertoPawn;

	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
		FName FirstAidKitRowName = "FirstAidKit";
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
		float FirstAidKitHealth = 35.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
		float DelayAfterUseFirstAidKit = 1.1f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|First Aid Kit")
		int32 MaxAmountOfFirstAidKitsOnHud = 99;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|First Aid Kit")
		TObjectPtr < USoundBase> UseFirstAidKitSound;

	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		FSettingSavedInJsonFile MouseSensitivityJSON = FSettingSavedInJsonFile("MouseSensitivity", 0.7f);
	UPROPERTY(EditAnywhere, Category = "Marine Pawn")
		FSettingSavedInJsonFile FieldOfViewJSON = FSettingSavedInJsonFile("FieldOfView", 90.f);

	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement")
		float MovementSpeed = 80000.f;
	UPROPERTY(EditAnywhere, Category = "Marine Pawn|Movement")
		float CounterMovementForce = 30.f;
	// When player wants to move forward and right then the forward speed is added to right speed and the player moves faster, this divider prevents this
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Movement")
		float ForwardAndRightAtTheSameTimeDivider = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Marine Pawn|Widgets")
		TSubclassOf<class UUserWidget> HUDClass;

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
		TObjectPtr<USoundBase> FootstepsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|Sounds")
		TObjectPtr<USoundBase> FootstepsWallrunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Footsteps|Sounds")
		TObjectPtr<USoundBase> FootstepsCroachSound;

	bool bIsDead;
	void PlayerDead();

	//Movement
	void Forward(float Axis);
	void Right(float Axis);
	void Move(FVector Direction, float Axis, const FName InputAxisName);
	FVector CalculateCounterMovement();
	float MovementForceDividerWhenInADS = 1.f;
	float MovementSpeedMutliplier = 1.f;
	const float DegreeForForwardVector = -90.f;

	//If true then in the the player will always walk forward
	bool bConstantlyGoForward; 

	//Footsteps sounds
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	bool CheckIfCanPlayFootstepsSound();
	void PlayFootstepsSound();
	FORCEINLINE void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	void CallAlbertosPressed();
	TObjectPtr<class UCraftingAlbertosWidget> CraftingWidget;

	void TakePressed();
	void TakeReleased();

	//FirstAidKit
	bool bCanUseFirstAidKit = true;
	FTimerHandle UseFirstAidKitHandle;
	void CanUseFirstAidKit() { bCanUseFirstAidKit = true; }
	void UseFirstAidKit();

	//Widgets
	void MakeHudWidget();
	UPROPERTY(Transient)
		TObjectPtr<UUserWidget> CrosshairWidget;
	UPROPERTY(Transient)
		TObjectPtr<class UHUDWidget> HudWidget;
	UPROPERTY(Transient)
		TObjectPtr<class AMarinePlayerController> MarinePlayerController;

};