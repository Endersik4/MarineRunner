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

	UFUNCTION(BlueprintPure)
		class UWallrunComponent* GetWallrunComponent() const { return WallrunComponent; }

	void ChangeMouseSensitivity(const FSettingSavedInJsonFile& NewMouseSensitivity, bool bResetMouseSensitivity = false);

	UPROPERTY(EditAnywhere, Category = "Alberto")
		class AAlbertosPawn* AlbertoPawn;

	FORCEINLINE FString GetFirstAidKitName() const { return FirstAidKits_Name; }
	FORCEINLINE float GetMovementForce() const { return MovementForce; }
	FORCEINLINE float GetHealth() const { return Health; }
	bool GetIsWallrunning() const;
	bool GetIsPlayerLerpingToHookLocation() const;
	bool GetIsCrouching() const;
	bool GetIsInSlowMotion() const;
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE bool GetShouldPlayerGoForward() const { return bShouldPlayerGoForward; }
	bool GetIsInAir() const;
	FVector GetCameraLocation() const;
	FORCEINLINE class UCapsuleComponent* GetPlayerCapsule() const { return CapsulePawn; }
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
	FORCEINLINE class UCroachAndSlide* GetCroachAndSlideComponent() const { return CroachAndSlideComponent; }
	FORCEINLINE const FSettingSavedInJsonFile& GetMouseSensitivityJSON() const { return MouseSensitivityJSON; }
	bool GetIsMessageDisplayed() const;
	bool GetIsInPauseMenu() const;

	FORCEINLINE void SetMovementForce(float NewForce) { MovementForce = NewForce; }
	FORCEINLINE void SetMovementSpeedMutliplier(float NewSpeed) { MovementSpeedMutliplier = NewSpeed; }
	FORCEINLINE void SetShouldPlayerGoForward(bool bShould) { bShouldPlayerGoForward = bShould; }

	void SetHealth(float NewHealth) { Health = NewHealth; }
	void SetQuickSelect(TMap < int32, class AGun* > NewWeaponsStorage);

	void MovementStuffThatCannotHappen(bool bShouldCancelGameplayThings = false);

	void UpdateHudWidget();

	void UpdateAlbertosInventory(bool bShouldUpdateInventory = true, bool bShouldUpdateCrafting = false);

	void MakeCrosshire(bool bShouldRemoveFromParent = false);

	bool bIsPlayerInElevator = false;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* CapsulePawn;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* ArmsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetInteractionComponent* WidgetInteractionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UInventoryComponent* InventoryComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UJumpComponent* JumpComponent;
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
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UArmsSwayComponent* ArmsSwayComponent;

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

	//Footstepts sounds
	void PlayFootstepsSound();
	bool bCanPlayFootstepsSound = true;
	FTimerHandle FootstepsHandle;
	void SetCanPlayFootstepsSound() { bCanPlayFootstepsSound = true; }

	void ReplaceRootComponentRotation();

	//Albertos
	void CallAlbertosPressed();
	class UCraftingAlbertosWidget* CraftingWidget;

	bool bIsDead;

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

	bool MakeCheckBox(FVector Size, FVector NewStart, FVector NewEnd, FHitResult& OutHitResult, bool bDebug = false);

	class AMarinePlayerController* MarinePlayerController;
};