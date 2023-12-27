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

	void ActivateHook(bool bActive = true);
	void ChangeToIdleAnim();
	void ChangeToPlayerInRangeAnim();

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		class USphereComponent* CheckSphere;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* HookMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UPaperFlipbookComponent* HookStateFlipBook;

	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings")
		float HookCooldownTime = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings")
		class UPaperFlipbook* HookIdleFlipBook;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings")
		class UPaperFlipbook* HookActivateFlipBook;

	void DelayForGrabbingTheHook();
};
