// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hook.generated.h"

UCLASS()
class MARINERUNNER_API AHook : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHook();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HookActivate();
	void HookInactiv();
	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class USphereComponent* CheckSphere;
private:

	//UFUNCTION()
	//	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//UFUNCTION()
	//	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* HookMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* M_PlayerInRange;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* M_PlayerOutRange;

	float Radius;
	void ChangeRadiusOfSphere();

	void SetPlayerPawn();
	class AMarineCharacter* MarinePawn;
};
