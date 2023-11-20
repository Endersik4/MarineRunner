// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoundOnHitComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API USoundOnHitComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoundOnHitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Sound On Hit Settings")
		float VelocityLengthErrorTolerance = 25.f;
	UPROPERTY(EditDefaultsOnly, Category = "Sound On Hit Settings")
		USoundBase* HitGroundSound;

	UFUNCTION()
		void OnHitPlaySound(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UAudioComponent* SpawnedHitGroundSound;
	AActor* HitActor;
};
