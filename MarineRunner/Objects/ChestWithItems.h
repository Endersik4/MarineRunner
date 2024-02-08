// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Door/UsePinToEnterInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "ChestWithItems.generated.h"

USTRUCT(BlueprintType)
struct FItemRandomSpawnStruct
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (ClampMin = "0.0", ClampMax = "100.0", UIMin = "0.0", UIMax = "100.0"))
		float PercentOfSpawningItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		int32 AmountItems = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
		FVector OffsetFromSpawnLocation;

	FItemRandomSpawnStruct()
	{
		PercentOfSpawningItem = 0.f;
		AmountItems = 1;
		OffsetFromSpawnLocation = FVector(0.f);
	}

	FItemRandomSpawnStruct(float Percent, int32 Amount, FVector Offset)
	{
		PercentOfSpawningItem = Percent;
		AmountItems = Amount;
		OffsetFromSpawnLocation = Offset;
	}
};

UCLASS()
class MARINERUNNER_API AChestWithItems : public AActor, public IUsePinToEnterInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChestWithItems();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ClickedOpenButton(class UDoorPanelWidget* ClickedWidget = nullptr) override;
	virtual void PinCorrect() override;
	virtual class UDoorPanelWidget* GetOtherPanelWidget(class UDoorPanelWidget* PanelActivatedByPlayer) override { return nullptr; }

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OpenChest();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* ChestSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* FrontPanelWidget;

	UPROPERTY(EditAnywhere, Category = "Chest Settings")
		UMaterialInstance* UpperOpenLockMaterial;
	UPROPERTY(EditAnywhere, Category = "Chest Settings")
		bool bUsePinCode;
	UPROPERTY(EditAnywhere, Category = "Chest Settings", meta = (EditCondition = "bUsePin", EditConditionHides, ClampMin = "1000", ClampMax = "9999", UIMin = "1000", UIMax = "9999"))
		int32 PinCode = 1111;

	UPROPERTY(EditAnywhere, Category = "Spawn Items")
		TMap<TSubclassOf<class APickupItem>, FItemRandomSpawnStruct> ItemsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
		UAnimationAsset* OpenChestAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* OpenChestSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* WrongCodeSound;

	bool bIsChestOpen;
	class UDoorPanelWidget* FrontChestPanelWidget;

	void SaveChestState(int32 SaveState);
	int32 CurrentUniqueID = 0;

};
