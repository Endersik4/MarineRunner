// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeadlyBox.generated.h"

UCLASS()
class MARINERUNNER_API ADeadlyBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADeadlyBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> DeadlyBox = nullptr;
	UPROPERTY(EditAnywhere, Category = "DeadlyBox Setup")
	float DamageToApply = 1000.f;
	UPROPERTY(EditAnywhere, Category = "DeadlyBox Setup")
	float ImpulseForceToApply = 0.f;

	UFUNCTION()
	void KillEverythingBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
