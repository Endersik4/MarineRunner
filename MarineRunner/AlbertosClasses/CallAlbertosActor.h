// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CallAlbertosActor.generated.h"

UCLASS()
class MARINERUNNER_API ACallAlbertosActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACallAlbertosActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* CallAlbertosBoxComp;

	UPROPERTY(EditAnywhere, Category = "Call Albertos Settings")
		FVector TeleportAlbertosLocation;

	UFUNCTION()
		void CallAlbertosBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bWasCalled = false;
};
