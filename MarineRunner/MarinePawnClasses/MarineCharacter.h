// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/InteractInterface.h"
#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameData.h"
#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

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

	FString GetFirstAidKitName() const { return FirstAidKits_Name; }
	float GetMovementForce() const { return MovementForce; }
	float GetHealth() const { return Health; }
	bool GetIsWallrunning() const;
	bool GetIsPlayerLerpingToHookLocation() const;
	bool GetIsCrouching() const;
	bool GetIsInSlowMotion() const;
	bool GetIsJumping() const { return bIsJumping; }
	bool GetIsOnRamp() const { return bSlideOnRamp; }
	bool GetIsGoingUp() const { return bIsGoingUp; }
	bool GetIsInAir() const { return bIsInAir; }
	class UHUDWidget* GetHudWidget() const { return HudWidget; }
	class UWeaponInventoryComponent* GetWeaponInventoryComponent() const { return WeaponInventoryComponent; }
	class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	class UCameraComponent* GetCamera() const { return Camera; }
	class UPauseMenuComponent* GetPauseMenuComponent()const { return PauseMenuComponent; }
	class UDashComponent* GetDashComponent() const { return DashComponent; }
	class UWeaponHandlerComponent* GetWeaponHandlerComponent() const { return WeaponHandlerComponent; }

	void SetMovementForce(float NewForce) { MovementForce = NewForce; }

	void SetHealth(float NewHealth) { Health = NewHealth; }
	void SetQuickSelect(TMap < int32, class AGun* > NewWeaponsStorage);

	void MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings = false);

	void UpdateHudWidget();

	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);

	void CallSaveGame(AActor* JustSavedCheckpoint = nullptr) { SaveGame(JustSavedCheckpoint); }
	bool CanPlayerSaveGame();
	void SpawnCannotSavedWidget() { SpawnPassingWidget(CannotSavedNotificationWidgetClass); }

	void LoadSavedSettingsFromGameInstance();

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
		float MovementForce = 7000.f;
	//Prevent from Sliding on the ground like Fish
	UPROPERTY(EditAnywhere, Category = "Movement")
		float CounterMovementForce = 4800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerOfMovementWhenADS = 1.4f;
	// Divide Movement speed and CounterMovementForce by this value when in Air
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float DividerForMovementWhenInAir = 10.f;

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

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> GameSavedNotificationWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
		TSubclassOf<class UUserWidget> CannotSavedNotificationWidgetClass;

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
	float CopyOfOriginalForce;

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
	void CallAlbertosPressed();
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

	//Crouching
	bool bSlideOnRamp;
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

	//Saving/Loading Game
	class USaveMarineRunner* CurrentSaveGameInstance;
	void SaveGame(AActor* JustSavedCheckpoint = nullptr);
	void LoadGame();
	void SpawnPassingWidget(const TSubclassOf<class UUserWidget> & WidgetClassToSpawn);

	//Widgets
	void MakeHudWidget();
	UUserWidget* CrosshairWidget;
	class UHUDWidget* HudWidget;

	//Checking Stuff
	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult &OutHitResult, bool bDebug = false);
	bool MakeCheckLine(FHitResult &OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug = false, FColor Color = FColor::Red);

	FVector EaseInQuint(FVector Start, FVector End, float Alpha);
	class AMarinePlayerController* MarinePlayerController;
};
