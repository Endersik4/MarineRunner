// Copyright Adam Bartela.All Rights Reserved

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

	FORCEINLINE bool GetIsPullingUp() const { return bIsPullingUp; }

private:
	//How Fast (in seconds) lerp to PullUp location will be done 
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpTime = 0.2f;
	//Forward Force that will be applied to player when he pull up on the edge
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpForceForward = 300000.f;
	//Up Force that will be applied to player when he pull up on the edge
	UPROPERTY(EditAnywhere, Category = "PullUp")
		float PullUpForceUp = 0.f;

	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		bool bDrawHelpersForPullup = false;

	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float CheckLinesForPullUpTime = 0.03f;
	//Z Location of Line that need to hit something for example: edge. This line must be true to Pullup
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupTrueLineZ = 100.f;
	//Z Location of Line that shouldnt hit something, This line must be false to Pullup
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupFalseLineZ = 130.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupLinesDistance = 100.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float PullupOffset_Z = 220.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float EdgeDetectionLineStartDistance = 100.f;
	UPROPERTY(EditAnywhere, Category = "PullUp|Check Raycast")
		float EdgeDetectionLineEndDistance = 200.f;

	FTimerHandle CheckIfShouldPullUpHandle;

	FVector PullupLocationZ;

	bool DetectEdge(const FVector& PlayerForwardVector);
	void StartPullUpOnEdge();
	/// <summary>
	/// Makes raycast to the edge to look for edge location
	/// </summary>
	/// <param name="EdgeLocation"> Vector is set by impact point of the raycast to the object </param>
	/// <returns> Returns true if object was hit by raycast, false otherwise </returns>
	bool GetEdgeLocationOfTheObject(const FVector& PlayerForwardVector, FVector & EdgeLocation);

	// Moving player to pull up location
	bool bIsPullingUp;
	float PullupTimeElapsed;
	FVector PlayerLocation;
	void MovePlayerToPullUpLocation(float Delta);

	// There is a bug where MarinePawn->GetActorForwardVector() is not correct
	const FVector CalculateForwardVectorForPlayer();

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn;
};
