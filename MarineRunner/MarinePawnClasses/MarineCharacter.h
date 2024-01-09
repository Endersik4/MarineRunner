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
	
	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class UStaticMeshComponent* CapsulePawn;

	UPROPERTY(EditAnywhere, Category = "Components", BlueprintReadWrite)
		class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class UInventoryComponent* InventoryComponent;

	UFUNCTION(BlueprintPure)
		class UWallrunComponent* GetWallrunComponent() const { return WallrunComponent; }

	UFUNCTION(BlueprintImplementableEvent)
		void RotateCameraWhileWallrunning(bool bIsRightSide = false);

	void ChangeMouseSensitivity(const FSettingSavedInJsonFile &NewMouseSensitivity, bool bResetMouseSensitivity = false);

	UPROPERTY(EditAnywhere, Category = "Alberto")
		class AAlbertosPawn* AlbertoPawn;

	//Is there Marine on The ground after begin in Air, It is used in Timeline in Blueprints
	UPROPERTY(VisibleAnywhere, Category = "Movement|Jump", BlueprintReadWrite)
		bool IsOnGround;

	FORCEINLINE FString GetFirstAidKitName() const { return FirstAidKits_Name; }
	FORCEINLINE float GetMovementForce() const { return MovementForce; }
	FORCEINLINE float GetHealth() const { return Health; }
	bool GetIsWallrunning() const;
	bool GetIsPlayerLerpingToHookLocation() const;
	bool GetIsCrouching() const;
	bool GetIsInSlowMotion() const;
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetIsOnRamp() const { return bIsOnRamp; }
	FORCEINLINE bool GetIsGoingUp() const { return bIsGoingUp; }
	FORCEINLINE bool GetIsInAir() const { return bIsInAir; }
	FVector GetCameraLocation() const;
	FORCEINLINE class UHUDWidget* GetHudWidget() const { return HudWidget; }
	FORCEINLINE class UWeaponInventoryComponent* GetWeaponInventoryComponent() const { return WeaponInventoryComponent; }
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE class UPauseMenuComponent* GetPauseMenuComponent()const { return PauseMenuComponent; }
	FORCEINLINE class UDashComponent* GetDashComponent() const { return DashComponent; }
	FORCEINLINE class UWeaponHandlerComponent* GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }
	FORCEINLINE class UMessageHandlerComponent* GetMessageHandlerComponent() const { return MessageHandlerComponent; }
	FORCEINLINE class USaveLoadPlayerComponent* GetSaveLoadPlayerComponent() const { return SaveLoadPlayerComponent; }
	FORCEINLINE const FSettingSavedInJsonFile& GetMouseSensitivityJSON() const { return MouseSensitivityJSON; }
	bool GetIsMessageDisplayed() const;
	bool GetIsInPauseMenu() const;

	void SetMovementForce(float NewForce) { MovementForce = NewForce; }
	void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMutliplier = NewSpeed; }

	void SetHealth(float NewHealth) { Health = NewHealth; }
	void SetQuickSelect(TMap < int32, class AGun* > NewWeaponsStorage);

	void MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings = false);

	void UpdateHudWidget();

	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);

	void MakeCrosshire(bool bShouldRemoveFromParent = false);

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
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPauseMenuComponent* PauseMenuComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USwingComponent* SwingComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWeaponHandlerComponent* WeaponHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USpawnDeathWidgetComponent* SpawnDeathWidgetComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UMessageHandlerComponent* MessageHandlerComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USaveLoadPlayerComponent* SaveLoadPlayerComponent;

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
		FSettingSavedInJsonFile MouseSensitivityJSON = FSettingSavedInJsonFile("MouseSensitivity", 0.7f);

	//Aka speed movement
	UPROPERTY(EditAnywhere, Category = "Movement")
		float MovementForce = 80000.f;
	//Prevent from Sliding on the ground like Fish
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CounterMovementForce = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerOfMovementWhenADS = 1.4f;
	// Divide Movement speed and CounterMovementForce by this value when in Air
	UPROPERTY(EditDefaultsOnly, Category = "Movement|In Air")
		float DividerForCounterForceWhenInAir = 13.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement|In Air")
		float DividerForMovementWhenInAir = 8.f;

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
		float DelayIsInAirTime = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CrosshairClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ImpactOnFloorSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* JumpSound;
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
	float InitialMovementForce;
	float MovementSpeedMutliplier = 1.f;
	const float DegreeForForwardVector = -90.f;


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

	void ReplaceRootComponentRotation();

	//Albertos
	void CallAlbertosPressed();
	class UCraftingAlbertosWidget* CraftingWidget;

	//Delay is in Air (can jump though player is not in the air)
	bool bDelayIsInAir;
	FTimerHandle DelayIsInAirHandle;
	void SetDelayIsInAir() { bDelayIsInAir = false; }

	bool bIsDead;

	//Delayed jump
	bool bCanDelayJump;
	FTimerHandle DelayJumpHandle;
	void DelayJump();
	void SetCanDelayJump() { bCanDelayJump = false; };

	//In Air
	bool bIsInAir;
	void CheckIfIsInAir();

	//Crouching
	bool bIsOnRamp;
	bool bIsGoingUp;

	//Taking Items
	void KeyEPressed();
	void KeyEReleased();

	//FirstAidKit
	bool bCanUseFirstAidKit = true;
	struct FItemStruct* FirstAidKitItem;
	FTimerHandle FirstAidKitHandle;
	void CanUseFirstAidKit() { bCanUseFirstAidKit = true; }
	void UseFirstAidKit();

	//Widgets
	void MakeHudWidget();
	UUserWidget* CrosshairWidget;
	class UHUDWidget* HudWidget;

	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult &OutHitResult, bool bDebug = false);

	class AMarinePlayerController* MarinePlayerController;
};
