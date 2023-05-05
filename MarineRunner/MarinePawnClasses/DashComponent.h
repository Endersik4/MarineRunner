// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UDashComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Dash();
private:
	//Dash Speed
	UPROPERTY(EditAnywhere, Category = "Movement")
		float DashForce = 8500.f;
	//Dash cooldown in Seconds
	UPROPERTY(EditAnywhere, Category = "Movement")
		float DashCoolDown = 1.4f;
	//How long Player Dash in Seconds (DashForce is applied per frame so after DashLength it will stop apply dash froce to the player)
	UPROPERTY(EditAnywhere, Category = "Movement")
		float DashLength = 0.03f;
	//How long DashWidget will be in player viewport
	UPROPERTY(EditAnywhere, Category = "Movement")
		float DashWidgetTime = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* DashSound;

	class AMarineCharacter* MarinePawn;

	bool bCanDash = true;
	float CopyForce;
	void DashLengthTimer();
	void DashCooldownTimer();
	FTimerHandle DashLengthHandle;
	FTimerHandle DashCooldownHandle;
		
};
