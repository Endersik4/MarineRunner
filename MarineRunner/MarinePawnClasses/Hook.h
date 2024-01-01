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
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		float StartChangingScaleDistance = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		float EndChangingScaleDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		FVector MinHookFlipbookScale = FVector(0.1f);
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		class UPaperFlipbook* HookIdleFlipBook;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		class UPaperFlipbook* HookActivateFlipBook;


	UFUNCTION()
		void OnCheckSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCheckSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DelayForGrabbingTheHook();
	FTimerHandle HookPressedHandle;

	bool bHookActive = false;
	bool bPlayerInRange;

	// change hook state flipbook scale according to player distance, closer == smaller
	AActor* PlayerInRange;
	FVector OriginalHookStateScale;
	void ChangeHookFlipbookScale(float Delta);
	void HideFlipbookIfItIsNotVisible();
	void ResetHookStateFlipbookScale();


};
