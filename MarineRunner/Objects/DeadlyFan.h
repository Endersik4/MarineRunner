// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadlyFan.generated.h"

UCLASS()
class MARINERUNNER_API ADeadlyFan : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeadlyFan();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DeadlyFanBaseMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* RotatingMeshAnchor;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DeadlyFanMesh;

	UPROPERTY(EditAnywhere, Category = "Fan Setup")
		float RotateSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "Fan Setup")
		USoundBase* FanSound;

	UFUNCTION()
		void OnFanMeshBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RotateFan(float Delta);
};
