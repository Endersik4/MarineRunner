// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"

#include "WallrunOnButton.generated.h"

UCLASS()
class MARINERUNNER_API AWallrunOnButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallrunOnButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* BaseMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* SocketRotateMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* MeshToRotateComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* ActivateRotateBoxComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UTextRenderComponent* ResetCurrentTimeText;

	// X = roll, Y = Pitch, Z = Yaw
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		class UCurveVector* RelativeRotationCurve;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		float ResetToInitialRotationTime = 6.f;

	UFUNCTION()
		void OnTimelineCallback(FVector NewRotation);
	UFUNCTION()
		void OnTimelineFinished();

	UFUNCTION()
		void OnActivateRotateBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& OutHit);

	bool bWasRotated = false;
	bool bResetingRotation;

	FTimerHandle ResetToInitialRotationHandle;
	FTimerHandle DisplayResetTimeHandle;
	int32 CurrentResetSecond = 0;
	void ResetRotateMeshTimeline();
	void ResetTimeSeconds();

	void StartRotateMeshTimeline();
	FTimeline RotateMeshTimeline;
};
