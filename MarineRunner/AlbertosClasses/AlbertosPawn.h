// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

#include "AlbertosPawn.generated.h"

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

	virtual void TakeItem(class AMarineCharacter* Character, bool& bIsItWeapon) override;
	virtual void ItemHover(class UHUDWidget* MarineHUDWidget) override;
	virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) override;
	virtual AActor* DropItem() override { return nullptr; }
	virtual bool ItemLocationWhenGrabbed(float SpeedOfItem) override { return false; }

	void CraftPressed(class APickupItem*, FTimerHandle* CraftTimeHandle);
	void CraftingFinished();

	UFUNCTION(BlueprintCallable)
		void SetIsFrontDoorOpen(bool bIsIt) { bIsFrontDoorOpen = bIsIt; }

	// Open or close the front door
	UFUNCTION(BlueprintImplementableEvent)
		void OpenFrontDoor(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldOpen = true);

	// Enable or disable craftign animation
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
		class USphereComponent* InventoryButtonComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_1;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_2;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* AlbertosFloatingMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float ActiveAlbertosRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosHoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosUnHoverMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float SpeedOfItemAfterCrafting = 5.f;
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

	bool bIsFrontDoorOpen;
	bool bShouldScaleCraftedItem;
	FVector TargetScaleOfCraftedItem;
	class APickupItem* CraftedItem;

	void ToggleVisibilityInventory(bool bCheckIfHidden = false);

	float CopyOfMaxSpeed;

	// When Players is near Albertos
	bool bPlayerWasClose;
	bool bCanRotateAlbertos;
	FTimerHandle PlayerIsNearHandle;
	void CheckIfThePlayerIsNear();
	void RotateAlbertosTowardsPlayer(float Delta);

	// Moving an item aftet it has been created
	bool bShouldMoveToFinalPosition;
	void InterpToFinalPosition(float Delta);

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

	FVector FinalLocation;

	class AAlbertosAIController* AlbertosAI;
};
