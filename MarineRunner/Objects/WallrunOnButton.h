// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "WallrunOnButton.generated.h"

UCLASS()
class MARINERUNNER_API AWallrunOnButton : public AActor, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallrunOnButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;
	virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) override;

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
		class UStaticMeshComponent* ActivateMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UTextRenderComponent* ResetCurrentTimeText;

	// X = roll, Y = Pitch, Z = Yaw
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		class UCurveVector* RelativeRotationCurve;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		float ResetToInitialRotationTime = 6.f;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		int32 IndexForMaterialToChange = 0;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor NotActiveMaterialColor;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor ActiveMaterialColor;

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

	FRotator InitialSocketRotation;

	void StartRotateMeshTimeline();
	FTimeline RotateMeshTimeline;

	UMaterialInstanceDynamic* ActiveDynamicMaterial;
};
