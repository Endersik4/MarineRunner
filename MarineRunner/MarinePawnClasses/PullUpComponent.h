// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PullUpComponent.generated.h"

/// <summary>
/// A Component that allow MarineCharacter to Pull Up on Any object 
/// In Details Panel you can set up variables for Pull Up 
/// </summary>
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UPullUpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPullUpComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPulledHimselfUp(bool bSet) { PulledHimselfUp = bSet; }

private:
	//How Fast (in seconds) lerp to PullUp location will be done 
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpTime = 0.23f;
	//Forward Force that will be applied to player when he pull up on the edge
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpForceForward = 500000.f;
	//Up Force that will be applied to player when he pull up on the edge
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpForceUp = 400000.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float CheckLinesForPullUpTime = 0.05f;
	//Z Location of Line that need to hit something for example: edge. This line must be true to Pullup
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupTrueLineZ = 150.f;
	//Z Location of Line that shouldnt hit something, This line must be false to Pullup
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupFalseLineZ = 210.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupLinesDistance = 100.f;

	FTimerHandle CheckIfShouldPullUpHandle;

	//EdgePullUp
	bool PulledHimselfUp;
	bool ShouldPullUpLerp;
	float PullupTimeElapsed;
	FVector PullupLocationZ;
	void PullupLerp();
	void EdgePullUp();
	FVector MarineLocation;

	class AMarineCharacter* MarinePawn;
	// There is a bug where MarinePawn->GetActorForwardVector() is not correct
	const FVector CalculateForwardVectorForPlayer();

	bool MakeCheckLine(FHitResult& OutHitResult, FVector NewStart, FVector NewEnd, bool bDebug = false, FColor Color = FColor::Red);

		
};
