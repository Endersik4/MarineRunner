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

	void StartMovingToHookLocation(const FVector & NewHookLocation, float NewSwingToHookLocationTime);
private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UNiagaraComponent> SwingLineTrail = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Swing Line Settings")
		float LifeSpan = 8.f;

	UPROPERTY(Transient)
		bool bStartMovingToHookLocation = false;
	UPROPERTY(Transient)
		float SwingToHookLocationTime = 0.f;
	UPROPERTY(Transient)
		float SwingToHookLocationTimeElapsed = 0.f;
	UPROPERTY(Transient)
		FVector HookLocation = FVector::Zero();
	UPROPERTY(Transient)
		FVector InitialSwingLineLocation = FVector::Zero();

	void MoveToHookLocation(float Delta);

};
