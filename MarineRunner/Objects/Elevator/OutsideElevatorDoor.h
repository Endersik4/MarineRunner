// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OutsideElevatorDoor.generated.h"

enum ECallElevatorAction
{
	ECEA_ShowCall,
	ECEA_HideCallAndShowWait,
	ECEA_HideCall,
	ECEA_ShowWaitEffect,
	ECEA_HideWaitEffect,
};

UCLASS()
class MARINERUNNER_API AOutsideElevatorDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutsideElevatorDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OpenOutsideElevatorDoor();
	void CloseOutsideElevatorDoor();

	void CallElevatorAction(ECallElevatorAction ActionToDo);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* OutsideElevatorDoorsSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* OutsideElevatorPanelMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* OutsideElevatorPanel;

	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings")
		class AElevator* ElevatorToCall;
	UPROPERTY(EditAnywhere, Category = "Outside Elevator Doors settings")
		int32 Floor;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Animations")
		UAnimationAsset* OpenElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Animations")
		UAnimationAsset* CloseElevatorDoorsAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Sounds")
		FName SoundLocationSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Outside Elevator Doors settings|Sounds")
		USoundBase* OpenElevatorDoorsSound;

	class UCallElevatorPanel* ElevatorPanelWidget;

	bool bDoorsOpen = false;
	void PlayElevatorEffects(UAnimationAsset* AnimToPlay, USoundBase* SoundToPlay);

};
