// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "DeadlyMovableBox.generated.h"

UCLASS()
class MARINERUNNER_API ABigHammer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABigHammer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootSceneComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> HammerMesh = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> HammerBoxComponent = nullptr;
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void MoveHammerTimelineProgress(float Value);
	UFUNCTION()
	void MoveHammerTimelineFinished();

	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
	float DamageToApply = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
	float ImpulseForceToApply = 0.f;
	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
	FVector DirectorToApplyCurve = FVector(0.f, 0.f, 1.f);
	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
	TObjectPtr<UCurveFloat> CurveZLocation = nullptr;
	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
	float TimeToPlayHitSound = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Hammer Sounds")
	TObjectPtr<USoundBase> HitSound = nullptr;

	UPROPERTY(Transient)
	FTimeline MoveObjectTimeline;
	void SetupMoveTimeline();

	UPROPERTY(Transient)
	FTimerHandle HitSoundHandle;
	void PlayHitSound();
};
