// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "PlayerConnectedComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UPlayerConnectedComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerConnectedComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void SprintPressed();
	void SprintReleased();

	void PossessedBy(AController* NewController);

	UFUNCTION()
	void SprintFOVTimelineProgress(float NewFOV);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos")
	TSubclassOf<class UAlbertosHUDWidget> AlbertosHUDClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos")
	TSubclassOf<UUserWidget> AlbertosHackHUDClass = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Movement")
	float MovementInputScale = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Movement")
	float MeshRotationSpeed = 6.f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Movement")
	FFloatRange LimitPitchRotation = FFloatRange(-30.f, 30.f);
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint")
	float SprintSpeed = 1400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint")
	float SprintAcceleration = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint")
	float SprintingTime = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint")
	float SprintRenewelTimeMultiplier = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint|FOV Effect")
	FTimeline SprintFOVTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint|FOV Effect")
	TObjectPtr<UCurveFloat> SprintStartCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Player Connected To Albertos|Sprint|FOV Effect")
	TObjectPtr<UCurveFloat> SprintStopCurve = nullptr;

	UPROPERTY(Transient)
	bool bPlayersPossessed = false;

	void RotateMeshTowardsCameraDir(float Delta);
	float GetCurrentYawMoveDirection();

	void Sprint(float Delta);
	UPROPERTY(Transient)
	bool bSprint = false;
	UPROPERTY(Transient)
	bool bIsSprinting = false;
	UPROPERTY(Transient)
	float CurrentSprintTime = 0.f;
	UPROPERTY(Transient)
	float OrginalAlbertosMaxSpeed = 800.f;
	UPROPERTY(Transient)
	float OrginalAcceleration = 2500.f;

	void SetUpPlayerAsAlbertos();
	UPROPERTY(Transient)
	TObjectPtr<class UAlbertosHUDWidget> AlbertosHUDWidget = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AAlbertosPawn> AlbertosOwner = nullptr;
};
