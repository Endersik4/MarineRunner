// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SwingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API USwingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool GetCanSwingLerp() const { return bCanSwingLerp; }

	void CallSwingPressed() { SwingPressed(); }

private:

	//Impuls before Interp
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingForce = 900.f;
	//Multiplier that is multiplying X and Y Velocity after Pawn got to Hook
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingLinearPhysicsMultiplier = 1.5f;
	//Pawn Interp Speed From Pawn location to Hook Locaiton
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingSpeed = 4.f;
	UPROPERTY(EditDefaultsOnly, Category = "Swing")
		float SwingDelay = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Actors to Spawn")
		TSubclassOf<class ASwingLine> SwingLineClass;

	class AMarineCharacter* MarinePawn;
	
	bool bCanMarineSwing;
	bool bCanSwingLerp;
	FVector HookLocation;
	FVector SwingImpulse;
	FTimerHandle SwingHandle;
	void Swing();
	void SwingPressed();
	void SwingInterp(float Delta);
	void SwingLineCheck();
	class AHook* HookCast;

		
};
