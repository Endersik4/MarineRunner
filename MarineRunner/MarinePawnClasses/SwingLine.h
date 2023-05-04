// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingLine.generated.h"

UCLASS()
class MARINERUNNER_API ASwingLine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwingLine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetHookLocation(FVector NewLocation) { HookLocation = NewLocation; }
	void SetSpeedLine(float NewSpeed) { LineTime = NewSpeed; }
	void SetCanTick(bool bCan) { bCanTick = bCan; }
private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UNiagaraComponent* LineTrail;

	bool bCanTick;
	FVector HookLocation;
	float LineTime;
	float TimeElapsed;
	void LineInterp();

};
