// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/TimelineComponent.h"
#include "MarineRunner/Interfaces/TakeInterface.h"

#include "DoorVent.generated.h"

UCLASS()
class MARINERUNNER_API ADoorVent : public AActor, public ITakeInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorVent();

	virtual void TakeItem(class AMarineCharacter* Character, bool& bIsItWeapon) override;
	virtual void ItemHover(class UHUDWidget* MarineHUDWidget) override;
	virtual void ItemUnHover(class UHUDWidget* MarineHUDWidget) override;
	virtual AActor* DropItem() override { return nullptr; }
	virtual bool MoveItemToEquipPosition(float SpeedOfItem) override { return false; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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
		bool TurnOnPhysicsAfterTheTimelineEnds = true;
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
};
