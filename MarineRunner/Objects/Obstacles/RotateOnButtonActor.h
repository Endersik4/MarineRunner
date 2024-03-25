// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MarineRunner/Player/SaveLoadGame/SaveCustomDataInterface.h"

#include "RotateOnButtonActor.generated.h"

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
		TObjectPtr<class UStaticMeshComponent> BaseMeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> SocketRotateMeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> MeshToRotateComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UBoxComponent> ActivateRotateBoxComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UStaticMeshComponent> ActivateMeshComponent = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UTextRenderComponent> ResetCurrentTimeText = nullptr;

	// X = roll, Y = Pitch, Z = Yaw
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		TObjectPtr<class UCurveVector> RelativeSocketRotationCurve = nullptr;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		float ResetToInitialRotationTime = 6.f;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		int32 IndexForActiveMaterialToChange = 0;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor NotActiveMaterialColor = FLinearColor::Red;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		FLinearColor ActiveMaterialColor = FLinearColor::Green;
	UPROPERTY(EditAnywhere, Category = "Rotation Settings")
		TObjectPtr<USoundBase> RotateObjectSound = nullptr;

	UFUNCTION()
		void RotateSocketTimelineProgress(FVector NewRotation);
	UFUNCTION()
		void RotateSocketTimelineFinished();

	UFUNCTION()
		void OnActivateRotateBoxHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& OutHit);

	UPROPERTY(Transient)
		FRotator InitialSocketRotation;
	UPROPERTY(Transient)
		bool bWasRotated = false;
	UPROPERTY(Transient)
		bool bResetingRotation = false;

	UPROPERTY(Transient)
		int32 CurrentResetSecond = 0;
	FTimerHandle ResetToInitialRotationHandle;
	FTimerHandle DisplayResetTimeHandle;
	void ResetRotateMeshTimeline();
	void ResetTimeSeconds();

	void StartRotateMeshTimeline();
	UPROPERTY(Transient)
		FTimeline RotateMeshTimeline;

	void SetUpActiveMaterial();

	UPROPERTY(Transient)
		TObjectPtr<UMaterialInstanceDynamic> ActiveDynamicMaterial = nullptr;
};
