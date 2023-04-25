// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TakeAndDrop.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UTakeAndDrop : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTakeAndDrop();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Take();
	void DropItem();

	UFUNCTION(BlueprintCallable)
	class AGun* GetGun() const { return Gun; }

private:
	UPROPERTY(EditAnywhere, Category = "Take Items")
		float TakeDistance = 400.f;
	UPROPERTY(EditAnywhere, Category = "Gun options")
		float SpeedOfComingGun = 2.f;

	class AMarineCharacter* MarinePawn;
	class AGun* Gun;

	bool bHaveItem;
	bool bIsInterpEnded = true;

	bool CheckIfPlayerCanTake(FHitResult& HitResult);
	void SetLocationOfItem(float Delta);
	void IsGunAtTheWeaponLocation();

	void ChangeToAnotherWeapon(int32 AmountOfWeapons);
		
};
