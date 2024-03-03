// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MarineRunner/Interfaces/TakeInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "AlbertosPawn.generated.h"

/// <summary>
/// A robot that will move around the player until the player is close enough to it. When he is located, the albertos will turn towards him.
/// The player can press the action button on the albertos when it is close to him, then the CraftingAlbertosWidget will appear.
/// After pressing the crafting button on the CraftingAlbertosWidget, the Albertos will open, play the crafting animation and play the crafting sound.
/// If the creating an item is completed, the item will be moved to the "FinalItemPosition" socket using InterpTo.
/// The robot can be called by the player, and when it does, Albertos will increase its speed and try to reach the player.
/// Albertos can play random sounds
/// </summary>
UCLASS()
class MARINERUNNER_API AAlbertosPawn : public APawn, public ITakeInterface, public ISaveCustomDataInterface
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

	// Save Custom Data Interface
	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject,const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject,const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE class UCraftItemAlbertosComponent* GetCraftItemAlbertosComponent() {return CraftItemAlbertosComponent;}
	FORCEINLINE class UPlayerIsNearAlbertosComponent* GetPlayerIsNearComponent() {return PlayerIsNearAlbertosComponent;}
	FORCEINLINE USkeletalMeshComponent* GetAlbertosSkeletal() const { return AlbertosSkeletalMesh; }
	UUserWidget* GetCraftingTableWidget() const;
	bool GetIsCraftingTableWidgetVisible() const;
	void CallToggleOpenDoor(bool bOpenDoor) const;

	void CallAlbertosToThePlayer(FVector PlayerLoc);
	void ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer = true);

	void ToggleInventoryVisibility();
	void ToggleVisibilityCraftingWidget();

	// Enable or disable crafting animation
	void EnableCraftingAnimation(bool bEnableCraftingAnim = true);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UChildActorComponent* DissolveBox_Left;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UChildActorComponent* DissolveBox_Right;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* AlbertosBoxComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* AlbertosSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* CraftingTableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* HologramMeshEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* AlbertosFloatingMovement;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCraftItemAlbertosComponent* CraftItemAlbertosComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCraftingWidgetAnimationComponent* CraftingWidgetAnimationComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPlayerIsNearAlbertosComponent* PlayerIsNearAlbertosComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		FFloatRange TimeRangeToPlayRandomSounds = FFloatRange(4.f, 10.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		USoundBase* RandomAlbertoSounds;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		USoundBase* AppearCraftingSound;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Crafting Animations")
		UAnimMontage* AlbertosCraftingAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Crafting Animations")
		float CraftingAnimationBlendOut = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Player")
		float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	// Teleports albertos to the player when the player is far away.
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Player")
		float MaxDistanceToPlayer = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Player")
		float TeleportToPlayerRadius = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Player")
		USoundBase* CallAlbertosSound;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Hover")
		int32 AlbertosHoverMaterialIndex = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Hover")
		UMaterialInstance* OnAlbertosHoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Hover")
		UMaterialInstance* OnAlbertosUnHoverMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Hover")
		USoundBase* HoverSound;

	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosAIController> AlbertosAI;

	float OriginalMoveSpeed;
	bool bIsHovered = false;

	// Random Sounds
	FTimerHandle RandomSoundHandle;
	void SetUpRandomSoundTimer();
	void PlayRandomAlbertoSound();

	/// <summary>
	/// Check if the distance to the player is greater than MaxDistanceToPlayer, if so, teleport Alberto near the player.
	/// </summary>
	/// <returns>True - if albertos is further from the player then MaxDistanceToPlayer <br/>
	/// False - otherwise</returns>
	bool TeleportAlbertosToPlayer(FVector& PlayerLoc);

	int32 CurrentUniqueID = 0;
};
