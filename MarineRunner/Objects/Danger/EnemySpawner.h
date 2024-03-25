// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "EnemySpawner.generated.h"

USTRUCT(BlueprintType)
struct FEnemiesSpawner {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AEnemyPawn> EnemyClassToSpawn;

	// Spawn EnemyClass in these locations
	UPROPERTY(EditAnywhere)
		TArray<FTransform> EnemiesTransform;

	FEnemiesSpawner()
	{
		EnemyClassToSpawn = nullptr;
		EnemiesTransform.Empty();
	}

	FEnemiesSpawner(TSubclassOf<class AEnemyPawn> & NewEnemyClassToSpawn, TArray<FTransform>& NewEnemiesTransform)
	{
		EnemyClassToSpawn = NewEnemyClassToSpawn;
		EnemiesTransform = NewEnemiesTransform;
	}
};

UCLASS()
class MARINERUNNER_API AEnemySpawner : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UBoxComponent> SpawnEnemiesBox = nullptr;

	UPROPERTY(EditAnywhere, Category = "Enemy spawner settings")
		TArray<FEnemiesSpawner> EnemiesSpawner = {FEnemiesSpawner()};
	UPROPERTY(EditAnywhere, Category = "Enemy spawner settings")
		float DelayToActivateEnemySpawner = 1.f;

	UFUNCTION()
		void OnSpawnEnemiesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(Transient)
		bool bEnemiesSpawned = false;
	void SpawnAllEnemiesFromSpawner();

	void EnemiesSpawnedSaveData();
	void DisableEnemySpawner(bool bDisable = true);

	FTimerHandle ActivateEnemySpawnerHandle;
	void EnableSpawnEnemiesBoxBeginOverlap();

	UPROPERTY(Transient)
		int32 CurrentUniqueID = 0;
};
