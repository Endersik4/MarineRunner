// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConveyorBelt.generated.h"

UCLASS()
class MARINERUNNER_API AConveyorBelt : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AConveyorBelt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void GenerateInstancedMeshesForConveyorBelt();
private:
	UFUNCTION()
	void OnMoveActorBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnMoveActorBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UInstancedStaticMeshComponent> ConveyorBeltInstancedMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> MoveActorBoxComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Conveyor Belt settings")
	bool bConveyorBeltActive = true;
	UPROPERTY(EditAnywhere, Category = "Conveyor Belt settings")
	float SpeedOfActorOnConveyorBelt = 200.f;
	UPROPERTY(EditAnywhere, Category = "Conveyor Belt settings|Mesh")
	int32 ConveyorBeltMeshAmount = 1;
	UPROPERTY(EditAnywhere, Category = "Conveyor Belt settings|Mesh")
	float ConveyorBeltMeshOffset = 100.f;
	UPROPERTY(EditAnywhere, Category = "Conveyor Belt settings|Mesh")
	FVector InstancedMeshDirection = FVector(1.f, 0.f, 0.f);

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> ActorsOnConveyorBelt = { nullptr };

	void CalculateBoxExtent();
	void CalculateBoxRelativeLocation();
	void MoveActors(float Delta);
};
