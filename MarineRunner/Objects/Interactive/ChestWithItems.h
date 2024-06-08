// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Objects/Interactive/PanelWithPin/UsePinToEnterInterface.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

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
	virtual	class UDoorPanelWidget* GetOtherPanelWidget(class UDoorPanelWidget* PanelActivatedByPlayer) override { return nullptr; }

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:
	void OpenChest();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> ChestSkeletalMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UWidgetComponent> FrontPanelWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = "Spawn Items")
	TMap<TSubclassOf<class APickupItem>, FItemRandomSpawnStruct> ItemsToSpawn;
	UPROPERTY(EditAnywhere, Category = "Spawn Items")
	FName SocketNameToSpawnItems = FName(TEXT("ItemSpawnLocation"));

	UPROPERTY(EditAnywhere, Category = "Chest Settings")
	int32 IndexToChangeOpenLockMaterial = 3;
	UPROPERTY(EditAnywhere, Category = "Chest Settings")
	TObjectPtr<UMaterialInstance>  UpperOpenLockMaterial = nullptr;
	UPROPERTY(EditAnywhere, Category = "Chest Settings")
	TObjectPtr<UMaterial>  UpperClosedLockMaterial = nullptr;
	UPROPERTY(EditAnywhere, Category = "Chest Settings|Pin")
	bool bUsePinCode = false;
	UPROPERTY(EditAnywhere, Category = "Chest Settings|Pin", meta = (EditCondition = "bUsePin", EditConditionHides, ClampMin = "1000", ClampMax = "9999", UIMin = "1000", UIMax = "9999"))
	int32 PinCode = 1111;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
	TObjectPtr<UAnimationAsset> OpenChestAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
	TObjectPtr<USoundBase> OpenChestSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Chest Settings")
	TObjectPtr<USoundBase> WrongCodeSound = nullptr;

	UPROPERTY(Transient)
	bool bIsChestOpen = false;
	void SetUpFrontChestPanelWidget();
	UPROPERTY(Transient)
	TObjectPtr<class UDoorPanelWidget> FrontChestPanelWidget = nullptr;;

	void SaveChestState(int32 SaveState);
	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;

};
