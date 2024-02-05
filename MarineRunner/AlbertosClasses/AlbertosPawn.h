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

	// Take Item Interface
	virtual void TakeItem(class AMarineCharacter* Character) override;
	virtual void ItemHover(class AMarineCharacter* Character) override;
	virtual void ItemUnHover(class AMarineCharacter* Character) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Open or close the front door
	UFUNCTION(BlueprintImplementableEvent)
		void OpenFrontDoor(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldOpen = true);

	// Enable or disable crafting animation
	UFUNCTION(BlueprintImplementableEvent)
		void EnableCraftingAnimation(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldPlayMontage = true);

	FORCEINLINE class UCraftItemAlbertosComponent* GetCraftItemAlbertosComponent() {return CraftItemAlbertosComponent;}
	FORCEINLINE UUserWidget* GetCraftingTableWidget() const;
	UFUNCTION(BlueprintPure)
		FORCEINLINE USkeletalMeshComponent* GetAlbertosSkeletal() const { return AlbertosSkeletalMesh; }
	FORCEINLINE void CallToggleOpenDoor(bool bOpenDoor) const;

	void CallAlbertoToThePlayer(FVector PlayerLoc);

	void ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer = true);

	void SetInventoryVisibility(bool bVisible = true);
	void ToggleVisibilityCraftingWidget();

	UPROPERTY(EditDefaultsOnly)
		class UChildActorComponent* DissolveBox_Left;
	UPROPERTY(EditDefaultsOnly)
		class UChildActorComponent* DissolveBox_Right;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* AlbertosBoxComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
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
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCraftItemAlbertosComponent* CraftItemAlbertosComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCraftingWidgetAnimationComponent* CraftingWidgetAnimationComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPlayerIsNearAlbertosComponent* PlayerIsNearAlbertosComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	// Teleports albertos to the player when the player is far away.
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float MaxDistanceToPlayer = 8000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float TeleportToPlayerRadius = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		FFloatRange TimeRangeToPlayRandomSounds = FFloatRange(4.f, 10.f);

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosHoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos| Hover")
		UMaterialInstance* OnAlbertosUnHoverMaterial;
		
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* RandomAlbertoSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* CallAlbertosSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* HoverSound;

	float OriginalMoveSpeed;
	bool bIsHovered = false;

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
	bool TeleportAlbertosToPlayer(FVector& PlayerLoc);

	class AAlbertosAIController* AlbertosAI;
};
