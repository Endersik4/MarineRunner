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
		TObjectPtr<class UStaticMeshComponent> BaseMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> SocketRotateMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> MeshToRotateComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UBoxComponent> ActivateRotateBoxComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> ActivateMeshComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UTextRenderComponent> ResetCurrentTimeText;

	// X = roll, Y = Pitch, Z = Yaw
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		TObjectPtr<class UCurveVector> RelativeSocketRotationCurve;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		float ResetToInitialRotationTime = 6.f;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		int32 IndexForActiveMaterialToChange = 0;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor NotActiveMaterialColor;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor ActiveMaterialColor;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		TObjectPtr<USoundBase> RotateObjectSound;

	UFUNCTION()
		void RotateSocketTimelineProgress(FVector NewRotation);
	UFUNCTION()
		void RotateSocketTimelineFinished();

	UFUNCTION()
		void OnActivateRotateBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& OutHit);

	FRotator InitialSocketRotation;
	bool bWasRotated = false;
	bool bResetingRotation;

	int32 CurrentResetSecond = 0;
	FTimerHandle ResetToInitialRotationHandle;
	FTimerHandle DisplayResetTimeHandle;
	void ResetRotateMeshTimeline();
	void ResetTimeSeconds();

	void StartRotateMeshTimeline();
	FTimeline RotateMeshTimeline;

	UPROPERTY(Transient)
		TObjectPtr<UMaterialInstanceDynamic> ActiveDynamicMaterial;
};
