// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

#include "AlbertosPawn.generated.h"

/// <summary>
/// A robot that will move around the player until the player is close enough to it. When he is located, the albertos will turn towards him.
/// The player can press the action button on the albertos when it is close to him, then the CraftingAlbertosWidget will appear.
/// After pressing the crafting button on the CraftingAlbertosWidget, the Albertos will open, play the crafting animation and play the crafting sound.
/// If the creating an item is completed, the item will be moved to the "FinalItemPosition" using InterpTo.
/// The robot can be called by the player, and when it does, Albertos will increase its speed and try to reach the player.
/// Albertos can play random sounds after TimeForRandomSound
/// </summary>
UCLASS()
class MARINERUNNER_API AAlbertosPawn : public APawn, public ITakeInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAlbertosPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UUserWidget* GetCraftingTableWidget() const;

	// Take Item Interface
	virtual void TakeItem(class AMarineCharacter* Character) override;
	virtual void ItemHover(class UHUDWidget* MarineHUDWidget) override;
	virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) override;

	void CraftPressed(class APickupItem*, FTimerHandle* CraftTimeHandle);
	void CraftingFinished();

	UFUNCTION(BlueprintCallable)
		void SetIsFrontDoorOpen(bool bIsIt) { bIsFrontDoorOpen = bIsIt; }

	// Open or close the front door
	UFUNCTION(BlueprintImplementableEvent)
		void OpenFrontDoor(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldOpen = true);

	// Enable or disable crafting animation
	UFUNCTION(BlueprintImplementableEvent)
		void EnableCraftingAnimation(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldPlayMontage = true, float ShouldEnable = 1.f);

	UFUNCTION(BlueprintPure)
		USkeletalMeshComponent* GetAlbertosSkeletal() const { return AlbertosSkeletalMesh; }

	void CallAlbertoToThePlayer(FVector PlayerLoc);

	void ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer = true);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* AlbertosBoxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* AlbertosSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* CraftingTableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* OpenInventoryBoxComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_1;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_2;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* AlbertosFloatingMovement;
	UPROPERTY(EditDefaultsOnly)
		class UChildActorComponent* DissolveBox_Left;
	UPROPERTY(EditDefaultsOnly)
		class UChildActorComponent* DissolveBox_Right;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float ActiveAlbertosRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	UPROPERTY(EditAnywhere, Category = "Setting up Albertos")
		bool bLookForPlayer = true;
	// Teleports albertos to the player when the player is far away.
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float MaxDistanceToPlayer = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float TeleportToPlayerRadius = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Crafting Widget Animation")
		float CraftingWidgetAnimationTime = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Crafting Widget Animation")
		FVector DissolveBoxesOffsetForAnim = FVector(0.f, 2130.f, 0.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosHoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosUnHoverMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float ItemMoveSpeedAfterCrafting = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		UMaterialInstance* OverlayCraftingMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds Variables")
		float TimeAfterStartingCraftSound = 1.515f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds Variables")
		float TimeOfCraftingRuntimeSound = 0.844f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting|Sounds Variables")
		float TimeLeftEndCraftingLoop = 1.341f;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* CraftingItemSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* RandomAlbertoSounds;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* OpenDoorSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* CallAlbertosSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* HoverSound;

	bool bIsFrontDoorOpen;
	bool bShouldScaleCraftedItem;
	FVector TargetScaleOfCraftedItem;
	class APickupItem* CraftedItem;

	void ToggleInventoryVisibility();
	void ToggleVisibilityCraftingWidget();

	float CopyOfMaxSpeed;

	// On Hovered Albertos
	bool bIsHovered = false;

	// When Players is near Albertos
	bool bPlayerWasClose;
	bool bCanAlbertosRotate;
	FTimerHandle PlayerIsNearHandle;
	void CheckIfThePlayerIsNear();
	void RotateAlbertosTowardsPlayer(float Delta);

	// Animation for opening/closing crafting table widget
	bool bPlayCraftingWidgetAnimation;
	bool bWasCraftingWidgetClosed;
	float CraftingWidgetAnimationTimeElapsed;
	FVector DissolveBox_Left_StartLoc;
	FVector DissolveBox_Left_EndLoc;
	FVector DissolveBox_Right_StartLoc;
	FVector DissolveBox_Right_EndLoc;
	void ToggleVisibilityForDissolveBoxes();
	void PrepareCraftingWidgetAnimation(bool bForwardAnim);
	void CraftingWidgetAnimation(float Delta);

	// Moving an item after it has been created
	bool bMoveCraftedItemToFinalPosition;
	void MoveCraftedItemToFinalPosition(float Delta);

	// Crafting Sounds
	FTimerHandle* CraftingTimeHandle;
	FTimerHandle ShouldLoopCraftingSoundHandle;
	class UAudioComponent* SpawnedCraftingSound;
	void ShouldLoopCraftingSound();

	// Random Sounds
	class UAudioComponent* SpawnedRandomSound;
	FTimerHandle RandomSoundHandle;
	float TimeForRandomSound;
	void PlayRandomAlbertoSound();

	/// <summary>
	/// Check if the distance to the player is greater than MaxDistanceToPlayer, if so, teleport Alberto near the player.
	/// </summary>
	/// <returns>True - if albertos is further from the player then MaxDistanceToPlayer <br/>
	/// False - otherwise</returns>
	bool TeleportAlbertosToPlayer(FVector & PlayerLoc);

	FVector FinalLocation;

	class AAlbertosAIController* AlbertosAI;
};
