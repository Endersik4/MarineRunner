// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "SpawnDestructibleActor.generated.h"

UCLASS()
class MARINERUNNER_API ASpawnDestructibleActor : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASpawnDestructibleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DestructibleMeshActorGotHit();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MainMeshComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	TSubclassOf <class ADestructibleMeshActor > DestructibleMeshActorClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Destruction Settings")
	float SpawnDestructibleMeshDelay = 0.08f;

	UPROPERTY(Transient)
	TObjectPtr<class ADestructibleMeshActor> SpawnedDestructibleMeshActor;

	UPROPERTY(Transient)
	bool bSaved = false;
	UPROPERTY(Transient)
	int32 CurrentUniqueID = 0;

	void AddActorToSavedDataObject();

	FTimerHandle SpawnDestructibleMeshHandle;
	void SpawnDestructibleMesh();
};
