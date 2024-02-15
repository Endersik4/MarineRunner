// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "Checkpoint.generated.h"

UCLASS()
class MARINERUNNER_API ACheckpoint : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:	

private:
	UFUNCTION()
		void OnCheckpointBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* CheckpointBox;

	UPROPERTY(EditDefaultsOnly, Category = "Checkpoint Setting")
		FString SaveToNameAfterCheckpoint;
	UPROPERTY(EditDefaultsOnly, Category = "Checkpoint Setting")
		FString SaveNumberWildCard;
	UPROPERTY(EditDefaultsOnly, Category = "Checkpoint Setting")
		float EnableCheckpointAtStartTime = 0.1f;

	bool bDisabledCheckpoint;
	void EnableCheckpointAfterDelay();

	int32 CurrentUniqueID = 0;
	void SaveCheckpoint();

	void DisableCheckpoint(bool bEnable = false);
};
