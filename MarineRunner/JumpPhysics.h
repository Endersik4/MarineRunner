// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "JumpPhysics.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UJumpPhysics : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJumpPhysics();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Jump();
	bool GetIsJumping() { return bShouldJump; }

private:
	UPROPERTY(EditAnywhere, Category = "JumpPhysics")
		float JumpVelocityStart = 10.f;
	UPROPERTY(EditAnywhere, Category = "JumpPhysics")
		float SubstructVelocityOnJumping = 2.f;
	UPROPERTY(EditAnywhere, Category = "JumpPhysics")
		float SubstructVelocityOnFalling = 2.f;

	
	void Jumping(float Delta);
	bool bShouldJump = false;
	double CopyJumpVelocityStart;

	float Falling = -200;
	void MarineFalling(float Delta);
	bool bIsFalling;
	int32 JumpsLeft = 2;
		
};
