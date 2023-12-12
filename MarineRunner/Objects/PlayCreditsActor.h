// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayCreditsActor.generated.h"

UCLASS()
class MARINERUNNER_API APlayCreditsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayCreditsActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UBoxComponent* CreditsBox;
	UPROPERTY(EditAnywhere, Category = "Start Credits Settings")
		FName CreditsLevelName = FName("CreditsMap");
	UPROPERTY(EditAnywhere, Category = "Start Credits Settings")
		float MoveToCreditsLevelTime = 2.f;
	UPROPERTY(EditAnywhere, Category = "Start Credits Settings")
		TSubclassOf<class UUserWidget> WidgetClassToSpawnWhenOverlap;

	UFUNCTION()
		void CreditsBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bCreditsActive = false;

	void SpawnWidget();

	FTimerHandle LoadCreditsMapHandle;
	void LoadCreditsMap();
};
