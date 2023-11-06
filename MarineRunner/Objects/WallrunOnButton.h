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

	// X = roll, Y = Pitch, Z = Yaw
	UPROPERTY(EditDefaultsOnly, Category = "Rotation Settings")
		class UCurveVector* RelativeRotationCurve;

	UFUNCTION()
		void OnTimelineCallback(FVector NewRotation);

	UFUNCTION()
		void OnActivateRotateBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& OutHit);

	bool bWasRotated = false;

	void StartRotateMeshTimeline();
	FTimeline RotateMeshTimeline;
};
