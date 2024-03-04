// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameData.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

#include "MarineCharacter.generated.h"

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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius = 0.f) override; //C++ ONLY

	void ChangeMouseSensitivity(const FSettingSavedInJsonFile& NewMouseSensitivity, bool bResetMouseSensitivity = false);
	void LoadFieldOfViewFromSettings();

	FORCEINLINE float GetMovementForce() const { return MovementForce; }
	FORCEINLINE float GetHealth() const { return Health; }
	bool GetIsWallrunning() const;
	bool GetIsPlayerLerpingToHookLocation() const;
	bool GetIsCrouching() const;
	bool GetIsInSlowMotion() const;
	bool GetIsInPullUpMode() const;
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetShouldPlayerGoForward() const { return bShouldPlayerGoForward; }
	bool GetIsInAir() const;
	FVector GetCameraLocation() const;
	FORCEINLINE class UCapsuleComponent* GetPlayerCapsule() const { return CapsulePawn; }
	FORCEINLINE class USceneComponent* GetGroundLocationSceneComponent() const { return GroundLocationSceneComponent; }
	FORCEINLINE class USceneComponent* GetRoofLocationSceneComponent() const { return RoofLocationSceneComponent; }
	FORCEINLINE class USkeletalMeshComponent* GetArmsSkeletalMesh() const { return ArmsSkeletalMesh; }
	FORCEINLINE class UHUDWidget* GetHudWidget() const { return HudWidget; }
	FORCEINLINE class UWeaponInventoryComponent* GetWeaponInventoryComponent() const { return WeaponInventoryComponent; }
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE class UPauseMenuComponent* GetPauseMenuComponent()const { return PauseMenuComponent; }
	FORCEINLINE class UDashComponent* GetDashComponent() const { return DashComponent; }
	FORCEINLINE class UWeaponHandlerComponent* GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }
	FORCEINLINE class UJumpComponent* GetJumpComponent() const { return JumpComponent; }
	FORCEINLINE class UPullUpComponent* GetPullUpComponent() const { return PullUpComponent; }
	FORCEINLINE class UMessageHandlerComponent* GetMessageHandlerComponent() const { return MessageHandlerComponent; }
	FORCEINLINE class USaveLoadPlayerComponent* GetSaveLoadPlayerComponent() const { return SaveLoadPlayerComponent; }
	FORCEINLINE class UArmsSwayComponent* GetArmsSwayComponent() const { return ArmsSwayComponent; }
	FORCEINLINE class UWallrunComponent* GetWallrunComponent() const { return WallrunComponent; }
	FORCEINLINE class UCroachAndSlide* GetCroachAndSlideComponent() const { return CroachAndSlideComponent; }
	FORCEINLINE class USlowMotionComponent* GetSlowMotionComponent() const { return SlowMotionComponent; }
	FORCEINLINE FSettingSavedInJsonFile& GetMouseSensitivityJSON()  { return MouseSensitivityJSON; }
	FORCEINLINE class AAlbertosPawn* GetAlbertosPawn() { return AlbertoPawn; }
	bool GetIsMessageDisplayed() const;
	bool GetIsInPauseMenu() const;

	FORCEINLINE void SetMovementForce(float NewForce) { MovementForce = NewForce; }
	FORCEINLINE void SetMovementForceDividerWhenInADS(float NewForceDivider) { MovementForceDividerWhenInADS = NewForceDivider; }
	FORCEINLINE void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMutliplier = NewSpeed; }
	FORCEINLINE void SetShouldPlayerGoForward(bool bShould) { bShouldPlayerGoForward = bShould; }
	FORCEINLINE void SetAlbertosPawn(class AAlbertosPawn* NewAlbertos) { AlbertoPawn = NewAlbertos; }

	void SetHealth(float NewHealth) { Health = NewHealth; }

	void MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings = false);

	void UpdateHudWidget();

	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);

	void MakeCrosshire(bool bShouldRemoveFromParent = false);

	bool bIsPlayerInElevator = false;

	UFUNCTION(BlueprintCallable)
		void PlayerDead();

	void ReplaceRootComponentRotation();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UTakeAndDrop* TakeAndDropComponent;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* CapsulePawn;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ArmsSkeletalMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USceneComponent* GroundLocationSceneComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USceneComponent* RoofLocationSceneComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UInventoryComponent* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UJumpComponent* JumpComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UCroachAndSlide* CroachAndSlideComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UDashComponent* DashComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UWallrunComponent* WallrunComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class USlowMotionComponent* SlowMotionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UPullUpComponent* PullUpComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UWeaponInventoryComponent* WeaponInventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UPauseMenuComponent* PauseMenuComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class USwingComponent* SwingComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UWeaponHandlerComponent* WeaponHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class USpawnDeathWidgetComponent* SpawnDeathWidgetComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UMessageHandlerComponent* MessageHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class USaveLoadPlayerComponent* SaveLoadPlayerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components", meta = (BlueprintSpawnableComponent))
		class UArmsSwayComponent* ArmsSwayComponent;

	UPROPERTY(EditAnywhere, Category = "Albertos")
		class AAlbertosPawn* AlbertoPawn;

	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float Health = 100.f;
	//Name of Item that will be used for healing. 
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		FName FirstAidKitRowName = "FirstAidKit";
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float FirstAidKitHealth = 35.f;
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float DelayAfterUseFirstAidKit = 1.1f;
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		FSettingSavedInJsonFile MouseSensitivityJSON = FSettingSavedInJsonFile("MouseSensitivity", 0.7f);
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		FSettingSavedInJsonFile FieldOfViewJSON = FSettingSavedInJsonFile("FieldOfView", 90.f);

	//Aka speed movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float MovementForce = 80000.f;
	//Prevent from Sliding on the ground like Fish
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CounterMovementForce = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerOfMovementWhenADS = 1.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* MarineHitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* UseFirstAidKitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsWallrunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsCroachSound;

	//Movement
	void Forward(float Axis);
	void Right(float Axis);
	void Move(FVector Direction, float Axis, const FName InputAxisName);
	FVector CalculateCounterMovement();
	float MovementSpeedMutliplier = 1.f;
	const float DegreeForForwardVector = -90.f;

	//If true then in the the player will always walk forward
	bool bShouldPlayerGoForward; 

	float MovementForceDividerWhenInADS = 1.f;

	//Footstepts sounds
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }


	//Albertos
	void CallAlbertosPressed();
	class UCraftingAlbertosWidget* CraftingWidget;

	bool bIsDead;

	//Taking Items
	void KeyEPressed();
	void KeyEReleased();

	//FirstAidKit
	bool bCanUseFirstAidKit = true;
	FTimerHandle UseFirstAidKitHandle;
	void CanUseFirstAidKit() { bCanUseFirstAidKit = true; }
	void UseFirstAidKit();

	//Widgets
	void MakeHudWidget();
	UUserWidget* CrosshairWidget;
	class UHUDWidget* HudWidget;

	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult& OutHitResult, bool bDebug = false);

	class AMarinePlayerController* MarinePlayerController;
};