// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingLine.generated.h"

/// <summary>
/// Niagara effect that will spawn when player press action key on Hook, when spawned then it will go to the hook location 
/// </summary>
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
	float LineTimeElapsed;
	void LineInterp(float Delta);

};
