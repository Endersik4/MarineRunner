// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/TimelineComponent.h"
#include "MarineRunner/Interfaces/TakeInterface.h"
#include "MarineRunner/SaveGame/SaveCustomDataInterface.h"

#include "DoorVent.generated.h"

UCLASS()
class MARINERUNNER_API ADoorVent : public AActor, public ITakeInterface, public ISaveCustomDataInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorVent();

	virtual void TakeItem(class AMarineCharacter* Character) override;
	virtual void ItemHover(class AMarineCharacter* Character) override;
	virtual void ItemUnHover(class AMarineCharacter* Character) override;

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
		class UStaticMeshComponent* DoorVentMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Door Vent Settings")
		FString DoorVentName = "Door Vent";
	UPROPERTY(EditDefaultsOnly, Category = "Door Vent Settings")
		FString DoorVentDesc = "desc";
	UPROPERTY(EditDefaultsOnly, Category = "Door Vent Settings")
		UTexture2D* DoorVentIcon;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings")
		FVector FinalLocation;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings")
		FRotator FinalRotation;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings")
		bool bVentDoorsBasedOnPhysics;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings")
		bool TurnOnPhysicsInsteadOfTimeline = false;
	// Add Curve values as local offset to actor or set curve values e.g SetActorRotation() 
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings", meta = (EditCondition = "!TurnOnPhysicsInsteadOfTimeline", EditConditionHides))
		bool bAddCurveValuesToVectors = true;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings|Open", meta = (EditCondition = "!TurnOnPhysicsInsteadOfTimeline", EditConditionHides))
		UCurveVector* DoorVentOpenLocationCurve;
	// X = Roll, Y = Pitch, Z = Yaw;
	UPROPERTY(EditAnywhere, Category = "Door Vent Settings|Open", meta = (EditCondition = "!TurnOnPhysicsInsteadOfTimeline", EditConditionHides))
		UCurveVector* DoorVentOpenRotationCurve;

	UFUNCTION()
		void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnOpenDoorVentLocProgress(FVector VectorValue);
	UFUNCTION()
		void OnOpenDoorVentLocFinished();

	FRotator GetRotationFromDoorVentOpenCurve();
	
	FTimeline OpenDoorVentLocTimeline;
	void PlayOpenDoorVentLocTimeline();

	bool bIsOpen = false;
	bool bIsHovered = false;

	void SaveCurrentStateOfVent(int32 CurrentState, FTransform ActoTransform);
	int32 CurrentUniqueID = 0;
};
