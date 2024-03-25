// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hook.generated.h"

UCLASS()
class MARINERUNNER_API AHook : public AActor
{
	GENERATED_BODY()
	
public:	
	AHook();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void StartHookCooldown();
	void ActivateHook(bool bActive = true);
	void ChangeToIdleAnim();
	void ChangeToPlayerInRangeAnim();

	FORCEINLINE bool GetCanGrabHook() const { return bCanGrabTheHook; }
	FORCEINLINE const FVector& GetHookLocationOffset() const { return HookLocationOffset; }
	FORCEINLINE const FVector& GetSwingLineLocationOffset() const { return SwingLineLocationOffset; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USphereComponent> HookActiveSphere = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> HookMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UPaperFlipbookComponent> HookStateFlipBook = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings")
		float HookCooldownTime = 1.5f;
	UPROPERTY(EditAnywhere, Category = "Hook Settings")
		FVector HookLocationOffset = FVector(0.f, 0.f, 350.f);
	UPROPERTY(EditAnywhere, Category = "Hook Settings")
		FRotator RotationOffsetToLookAtThePlayer = FRotator(0.f, 90.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Hook Settings|Detection")
		float CheckIfHookIsVisibleInterval = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Hook Settings|Swing")
		FVector SwingLineLocationOffset = FVector(0.f, 0.f, 150.f);
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		float StartChangingScaleDistance = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		float EndChangingScaleDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		FVector MinHookFlipbookScale = FVector(0.1f);
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		TObjectPtr<class UPaperFlipbook> HookIdleFlipBook = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Hook Settings|Flipbook")
		TObjectPtr<class UPaperFlipbook> HookActivateFlipBook = nullptr;

	UFUNCTION()
		void OnCheckSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCheckSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DelayForGrabbingTheHook();
	FTimerHandle HookPressedHandle;

	UPROPERTY(Transient)
		bool bHookActive = false;
	UPROPERTY(Transient)
		bool bPlayerInRange = false;
	UPROPERTY(Transient)
		bool bCanGrabTheHook = true;

	FTimerHandle HookVisibleHandle;

	// change hook state flipbook scale according to player distance, closer == smaller
	UPROPERTY(Transient)
		TObjectPtr<AActor> PlayerInRange = nullptr;

	UPROPERTY(Transient)
		FVector OriginalHookStateScale = FVector::Zero();

	void ChangeHookFlipbookScale(float Delta);
	void HookFlipbookLookAtThePlayer(float Delta);
	void HideFlipbookIfItIsNotVisible();
	void ResetHookStateFlipbookScale();

};
