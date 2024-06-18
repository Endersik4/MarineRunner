// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDisabler.generated.h"

UCLASS()
class MARINERUNNER_API AEnemyDisabler : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDisabler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> DisableEnemiesBox = nullptr;

	UFUNCTION()
	void OnDisableEnemiesBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroyAllEnemiesOnMap();
	void ResetDetectedEnemies();
};
