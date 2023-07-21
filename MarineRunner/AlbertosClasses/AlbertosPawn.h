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
	virtual AActor* DropItem() override { return nullptr; }
	virtual bool ItemLocationWhenGrabbed(float SpeedOfItem) override { return false; }

	void CraftPressed(class APickupItem*);
	void CraftingFinished();

	UFUNCTION(BlueprintCallable)
		void SetIsFrontDoorOpen(bool bIsIt) { bIsFrontDoorOpen = bIsIt; }

	// Open or close the front door
	UFUNCTION(BlueprintImplementableEvent)
		void OpenFrontDoor(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldOpen = true);

	// Enable or disable craftign animation
	UFUNCTION(BlueprintImplementableEvent)
		void EnableCraftingAnimation(USkeletalMeshComponent* AlbertosSkeleton, bool bShouldPlayMontage = true, float ShouldEnable = 1.f);

	USkeletalMeshComponent* GetAlbertosSkeletal() const { return AlbertosSkeletalMesh; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* AlbertosCapsuleComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* AlbertosSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* CraftingTableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_1;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Hologram_2;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* AlbertosFloatingMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		float SpeedOfItemAfterCrafting = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Crafting")
		UMaterialInstance* OverlayCraftingMaterial;

	bool bIsFrontDoorOpen;
	class APickupItem* CraftedItem;

	// Moving an item aftet it has been created
	bool bShouldMoveToFinalPosition;
	void InterpToFinalPosition(float Delta);

	FVector FinalLocation;

};
