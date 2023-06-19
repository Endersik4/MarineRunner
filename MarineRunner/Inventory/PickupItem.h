// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

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
	virtual AActor* DropItem() override { return nullptr; }
	virtual bool ItemLocationWhenGrabbed(float SpeedOfItem) override { return false; }

	FString GetItemName() const { return Item_Name; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		FString Item_Name = "ColtAmmo";
	UPROPERTY(EditDefaultsOnly, Category = "Item Settings")
		int32 Amount;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PickUpSound;
};
