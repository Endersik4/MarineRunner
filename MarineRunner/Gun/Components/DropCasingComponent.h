// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DropCasingComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UDropCasingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDropCasingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void StartTimerToDropCasing();
	void CancelDropCasingTimer();

private:

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection")
		bool bCasingEjection = true;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		FName SocketNameLocationToSpawnCasing = "BulletDrop";
	// after shooting use timer to spawn casing 
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		float SpawnCasingAfterTime = 0.05f;
	//Actor that will spawn on the location from Socket "BulletDrop". Its for casing that is dumped from gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		TSubclassOf<AActor> DropBulletClass;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection", EditConditionHides))
		bool bShouldRandomizeRotationOfCasing = true;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Roll = FFloatRange(-10.f, 10.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Pitch = FFloatRange(-15.f, 15.f);
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Bullet|Casing Ejection", meta = (EditCondition = "bCasingEjection && bShouldRandomizeRotationOfCasing", EditConditionHides))
		FFloatRange RandomCasingRotation_Yaw = FFloatRange(-40.f, 10.f);

	void DropCasing();
	FTimerHandle DropCasingHandle;

	UPROPERTY(Transient)
		TObjectPtr<class AGun> Gun;
};
