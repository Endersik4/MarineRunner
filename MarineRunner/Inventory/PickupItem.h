// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/TakeInterface.h"
#include "MarineRunner/Inventory/InventoryComponent.h"

#include "PickupItem.generated.h"

UCLASS()
class MARINERUNNER_API APickupItem : public AActor, public  ITakeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TakeItem(class AMarineCharacter* Character, bool& bIsItWeapon) override;
	virtual void ItemHover(class UHUDWidget* MarineHUDWidget) override;
	virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) override;
	virtual AActor* DropItem() override { return nullptr; }
	virtual bool MoveItemToEquipPosition(float SpeedOfItem) override { return false; }

	void SetItemAmount(int32 NewAmount) { ItemSettings.Item_Amount = NewAmount; }

	virtual void ChangeSimulatingPhysics(bool bChange = true) { ; };

	FString GetItemName() const { return ItemSettings.Item_Name; }
	FItemStruct GetItemSettings() const { return ItemSettings; }

	virtual void SetDissolveMaterial(UMaterialInstance* NewMaterial, USkeletalMeshComponent* SkeletalMesh = nullptr);

	virtual void SetCollisionNewResponse(ECollisionChannel ChannelName, ECollisionResponse NewResponse);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USoundOnHitComponent* SoundOnHitComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PickUpSound;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		FItemStruct ItemSettings;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		float DissolveStartValue = 0.6f;
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		float DissolveEndValue = -0.4f;

	//Dissolve Material
	bool bShouldDissolve;
	float DissolveTimeElapsed;
	UMaterialInstanceDynamic* DissolveDynamicMaterial;
	USkeletalMeshComponent* WeaponSkeletalMesh;
	void Dissolve(float Delta);

	


};
