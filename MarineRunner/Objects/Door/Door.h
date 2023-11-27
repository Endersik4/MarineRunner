// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class MARINERUNNER_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OpenDoor();
	void CloseDoor();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USkeletalMeshComponent* DoorSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* DoorPanelMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* DoorPanelWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "Door Settings")
		float CloseDoorAfterInactivityTime = 4.f;
	UPROPERTY(EditAnywhere, Category = "Door Settings")
		bool bUsePin = false;
	UPROPERTY(EditAnywhere, Category = "Door Settings", meta = (EditCondition = "bUsePin", EditConditionHides))
		int32 Pin;

	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Animations")
		UAnimationAsset* OpenDoorAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Animations")
		UAnimationAsset* CloseDoorAnim;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		FName DoorSoundSocketName;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		USoundBase* OpenDoorSound;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings|Sounds")
		USoundBase* CloseDoorSound;

	bool bDoorOpen = false;

	class UDoorPanelWidget* DoorPanelWidget;
	void SetUpDoorPanel();
};
