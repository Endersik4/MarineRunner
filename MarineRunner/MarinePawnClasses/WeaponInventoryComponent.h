// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponInventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UWeaponInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddNewWeaponToStorage(class AGun* NewGun);
	void RemoveWeaponFromStorage(class AGun* EquipedGun);

	class AGun* GetWeaponFromStorage(int32 KeyForWeapon, class AGun* CurrentWeapon);

private:
	TMap < int32, class AGun* > WeaponsStorage;
		
	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();
};
