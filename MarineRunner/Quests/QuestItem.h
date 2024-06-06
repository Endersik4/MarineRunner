// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/Interfaces/TakeInterface.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "QuestItem.generated.h"

UCLASS()
class MARINERUNNER_API AQuestItem : public AActor, public ITakeInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	AQuestItem();

protected:
	virtual void BeginPlay() override;

	virtual void TakeItem(class AMarineCharacter* Character) override;
	virtual void ItemHover(class AMarineCharacter* Character) override;
	virtual void ItemUnHover(class AMarineCharacter* Character) override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> QuestItemMesh;
	
	UPROPERTY(EditAnywhere, Category = "Quest Item Settings")
	FName AttachItemToSocketName = FName(TEXT("Weapon_R"));
	UPROPERTY(EditAnywhere, Category = "Quest Item Settings")
	TSubclassOf<AActor> QuestActorToAttach = nullptr;
	UPROPERTY(EditAnywhere, Category = "Quest Item Settings")
	TObjectPtr<UAnimationAsset> QuestAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quest Item Settings|Information On Hover")
	FString MessageName = FString("default message name");
	UPROPERTY(EditDefaultsOnly, Category = "Quest Item Settings|Information On Hover")
	FString MessageDescription = FString("default message description");
	UPROPERTY(EditDefaultsOnly, Category = "Quest Item Settings|Information On Hover")
	TObjectPtr<UTexture2D> MessageIcon = nullptr;

	UPROPERTY(Transient)
	bool bQuestWasPlayed = false;
	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedQuestActor = nullptr;

	FTimerHandle StartQuestAnimHandle;
	void StartQuestAnim(class AMarineCharacter* Player);

	FTimerHandle EndQuestAnimHandle;
	void EndQuestAnim(class AMarineCharacter* Player);

	void AddActorToSavedDataObject();

	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;
};
