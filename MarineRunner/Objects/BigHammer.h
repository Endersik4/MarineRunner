// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "BigHammer.generated.h"

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
		class UStaticMeshComponent* HammerMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* HammerBoxComponent;
	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void MoveHammerTimelineProgress(float Value);
	UFUNCTION()
		void MoveHammerTimelineFinished();

	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
		UCurveFloat* CurveZLocation;
	UPROPERTY(EditAnywhere, Category = "Hammer Setup")
		float TimeToPlayHitSound = 0.4f;
	UPROPERTY(EditDefaultsOnly, Category = "Hammer Sounds")
		USoundBase* HitSound;

	FTimeline MoveObjectTimeline;
	void SetupMoveTimeline();

	FTimerHandle HitSoundHandle;
	void PlayHitSound();
};
