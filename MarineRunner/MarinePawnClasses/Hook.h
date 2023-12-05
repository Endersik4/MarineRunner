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

	bool bCanGrabTheHook = true;
	
	void StartHookCooldown();
	void HookActivate(bool bActive = true);

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class USphereComponent* CheckSphere;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* HookMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings")
		int32 PlayerInRangeIndexMaterial = 0;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* M_PlayerInRange;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInstance* M_PlayerOutRange;

	float CopyOfSphereRadius;
	void DelayForGrabbingTheHook();

	void SetPlayerPawn();
	class AMarineCharacter* MarinePawn;
};
