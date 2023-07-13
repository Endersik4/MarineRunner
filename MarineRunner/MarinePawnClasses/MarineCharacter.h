// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/InteractInterface.h"

#include "MarineCharacter.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class UStaticMeshComponent* CapsulePawn;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
		class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsInputDisabled;

	UFUNCTION(BlueprintPure)
		class UWallrunComponent* GetWallrunComponent() const { return WallrunComponent; }

	UFUNCTION(BlueprintCallable)
		void SetShouldDieWhenDash(bool bShould) { bShouldDieWhenDash = bShould; }

	UFUNCTION(BlueprintImplementableEvent)
		void RotateCameraWhileWallrunning(bool bIsRightSide = false);

	UFUNCTION(BlueprintImplementableEvent)
		void ChangeMouseSensivity(float NewMouseSensivity);

	//Is there Marine on The ground after begin in Air, It is used in Timeline in Blueprints
	UPROPERTY(VisibleAnywhere, Category = "Movement|Jump", BlueprintReadWrite)
		bool IsOnGround;

	FString GetFirstAidKitName() const { return FirstAidKits_Name; }
	float GetMovementForce() const { return MovementForce; }
	float GetHealth() const { return Health; }
	bool GetIsWallrunning() const;
	bool GetIsJumping() const { return bIsJumping; }
	bool GetIsOnRamp() const { return bSlideOnRamp; }
	bool GetIsGoingUp() const { return bIsGoingUp; }
	bool GetIsInAir() const { return bIsInAir; }
	bool GetShouldAddCounterMovement() const { return bShouldAddCounterMovement; }
	class UHUDWidget* GetHudWidget() const { return HudWidget; }
	class UWeaponInventoryComponent* GetWeaponInventoryComponent() const { return WeaponInventoryComponent; }
	class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	class UCameraComponent* GetCamera() const { return Camera; }
	class AGun* GetGun() const { return Gun; }

	void SetMovementForce(float NewForce) { MovementForce = NewForce; }
	void SetGun(class AGun* NewGun) { Gun = NewGun; }
	void SetCanChangeWeapon(bool bCan) { bCanChangeWeapon = bCan; }
	void SetIsCroaching(bool bIs) { bIsCroaching = bIs; }

	void CallSaveGame(FVector CheckpointLocation) { SaveGame(CheckpointLocation); }
	void CallADSReleased() { ADSReleased(); }
	void SetHealth(float NewHealth) { Health = NewHealth; }
	void SetQuickSelect(TMap < int32, class AGun* > NewWeaponsStorage);

	void SetMovementImpulse(FVector NewImpulse) { MovementImpulse = NewImpulse; }
	void SetShouldAddCounterMovement(bool bShould) { bShouldAddCounterMovement = bShould; }
	void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMultiplier = NewSpeed; }

	void MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings = false);
	void HideGunAndAddTheNewOne(class AGun* NewGun);

	void MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov = true, bool bAddChromaticAbberation = true);
	void RemoveDashWidget();
	void UpdateHudWidget();

	void UpdateAlbertosInventory(bool bShouldUpdateCrafting = false);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCroachAndSlide* CroachAndSlideComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UDashComponent* DashComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UTakeAndDrop* TakeAndDropComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWallrunComponent* WallrunComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USlowMotionComponent* SlowMotionComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPullUpComponent* PullUpComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWeaponInventoryComponent* WeaponInventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditAnywhere, Category = "Alberto")
		class AAlbertosPawn* AlbertoPawn;

	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float Health = 100.f;
	//Name of Item that will be used for healing. 
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		FString FirstAidKits_Name = "FirstAidKit";
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float FirstAidKitHealth = 35.f;
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float DelayAfterUseFirstAidKit = 1.1f;
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		float MouseSensivity = 0.7f;
	UPROPERTY(EditAnywhere, Category = "Set Up Marine Pawn")
		TArray<float> MouseSensivityWhenScope = {0.4f, 0.2f, 0.1f, 0.05f};

	//Aka speed movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float MovementForce = 7000.f;
	//Prevent from Sliding on the ground like Fish
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CounterMovementForce = 4800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerOfMovementWhenADS = 1.4f;

	//InitialJumpForce is lerping to -50.f and then applied to Velocity Z
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float InitialJumpForce = 1800.f;
	//Impuls force that is applied to Player in down vector, Thanks to this player is falling faster
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float JumpDownForce = 15000.f;
	//How Fast (in seconds) lerp will be done 
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float JumpUpTime = 0.35f;
	//Time for Delay Jump
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float DelayJumpTime = 0.15f;
	UPROPERTY(EditAnywhere, Category = "Movement|Jump")
		float DelayIsInAirTime = 0.55f;

	//Impuls before Interp/*
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingForce = 900.f;
	//Multiplier that is multiplying X and Y Velocity after Pawn got to Hook
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingLinearPhysicsMultiplier = 1.5f;
	//Pawn Interp Speed From Pawn location to Hook Locaiton
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingSpeed = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Swing")
		float SwingDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Actors to Spawn")
		TSubclassOf<class ASwingLine> SwingLineClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> DashClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSInSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ADSOutSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ImpactOnFloorSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* JumpSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* QuickSelectSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* MarineHitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* UseFirstAidKitSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* SwingSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsWallrunSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds|Footsteps")
		USoundBase* FootstepsCroachSound;

	//Movement
	float MovementSpeedMultiplier = 1.f;
	float CopyOfOriginalForce;
	bool bShouldAddCounterMovement = true;
	FVector MovementImpulse;
	void Movement(float Delta);
	void DisableCounterMovement(FVector& MovementDir);
	void UnstickFromWall(FVector& ForwardDir, FVector& RightDir);
	void GoConstanlyForward(FVector& ForwardDir, FVector& RightDir);

	//Footstepts sounds
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() {bCanPlayFootstepsSound = true;}

	//Jumps Variables
	bool bDownForce;
	bool bIsJumping;
	float JumpTimeElapsed;
	void Jump();
	void JumpTick(float DeltaTime);

	//Albertos
	class UCraftingAlbertosWidget* CraftingWidget;

	//Delay is in Air (can jump though player is not in the air)
	bool bDelayIsInAir;
	FTimerHandle DelayIsInAirHandle;
	void SetDelayIsInAir() { bDelayIsInAir = false; }

	//Delayed jump
	bool bCanDelayJump;
	FTimerHandle DelayJumpHandle;
	void DelayJump();
	void SetCanDelayJump() { bCanDelayJump = false; };

	//In Air
	bool bIsInAir;
	void CheckIfIsInAir();

	//Dashing
	bool bShouldDieWhenDash;
	void Dash();

	//Croaching
	bool bIsCroaching;
	bool bSlideOnRamp;
	bool bIsGoingUp;
	void CroachPressed();
	void CroachReleased();

	//Taking Items
	void KeyEPressed();
	void KeyEReleased();
	void DropItem();

	//Gun
	void Shoot();
	void ReleasedShoot();
	void Reload();
	void Zoom(float WheelAxis);
	int32 CurrentScopeIndex;
	class AGun* Gun;

	//FirstAidKit
	bool bCanUseFirstAidKit = true;
	struct FItemStruct* FirstAidKitItem;
	FTimerHandle FirstAidKitHandle;
	void CanUseFirstAidKit() { bCanUseFirstAidKit = true; }
	void UseFirstAidKit();

	//Aiming
	void ADSPressed();
	void ADSReleased();
	bool bIsPlayerADS;

	//Weapon Inventory
	void FirstWeapon();
	void SecondWeapon();
	bool bCanChangeWeapon = true;
	
	//Swing
	bool bIsSwingPressed = false;
	bool bCanMarineSwing;
	bool bShouldCheckForSwing = true;
	bool bCanSwingLerp;
	FVector HookLocation;
	FVector SwingImpulse;
	FTimerHandle SwingHandle;
	void SwingPressed();
	void Swing();
	void SwingInterp();
	void SwingLineCheck();
	class AHook* HookCast;

	//SlowMotion
	void SlowMotionPressed();

	//Saving/Loading Game
	void SaveGame(FVector CheckpointLocation = FVector(0));
	void LoadGame();

	//Widgets
	void MakeHudWidget();
	void MakeCrosshire();
	UUserWidget* CrosshairWidget;
	class UDashWidget* DashWidget;
	class UHUDWidget* HudWidget;

	//Checking Stuff
	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult &OutHitResult, bool bDebug = false);
	bool MakeCheckLine(FHitResult &OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug = false, FColor Color = FColor::Red);

	FVector EaseInQuint(FVector Start, FVector End, float Alpha);
	APlayerController* MarinePlayerController;
};
