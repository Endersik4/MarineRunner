// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CraftItemAlbertosComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UCraftItemAlbertosComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCraftItemAlbertosComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void SetIsFrontDoorOpen(bool bIsIt) { bIsFrontDoorOpen = bIsIt; }

	void CraftPressed(class AMarineCharacter* Player, class APickupItem*, FTimerHandle* CraftTimeHandle);
	void CraftingFinished();


private:
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float ItemMoveSpeedAfterCrafting = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		FName ItemSpawnLocationSocketName = "ItemSpawnLocation";
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

	bool bIsFrontDoorOpen;
	bool bShouldScaleCraftedItem;
	FVector TargetScaleOfCraftedItem;
	class APickupItem* CraftedItem;

	// Crafting Sounds
	FTimerHandle* CraftingTimeHandle;
	FTimerHandle ShouldLoopCraftingSoundHandle;
	class UAudioComponent* SpawnedCraftingSound;
	void ShouldLoopCraftingSound();

	// Moving an item after it has been created
	bool bMoveCraftedItemToFinalPosition;
	void MoveCraftedItemToFinalPosition(float Delta);

	class AAlbertosPawn* AlbertosPawn;
		
};
