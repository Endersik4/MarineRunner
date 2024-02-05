// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerIsNearAlbertosComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UPlayerIsNearAlbertosComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerIsNearAlbertosComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleOpenDoor(bool bOpenDoor);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float ActiveAlbertosRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos")
		float TimeToCheckIfPlayerIsNear = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Setting up Albertos")
		FName OpenDoorSoundSocketName = FName(TEXT("Wysuwak"));
	UPROPERTY(EditAnywhere, Category = "Setting up Albertos")
		bool bIgnorePlayer = true;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* OpenDoorSound;

	bool bIsFrontDoorOpen;


	// When Players is near Albertos
	bool bPlayerIsClose;
	bool bRotateAlbertosTowardPlayer;
	FTimerHandle PlayerIsNearHandle;
	void CheckIfThePlayerIsNear();
	void GoBackToWendering();
	void PlayerIsCloseToAlbertos();
	void RotateAlbertosTowardsPlayer(float Delta);

	class AAlbertosPawn* AlbertosOwner;
	class AAlbertosAIController* AlbertosAI;
	APawn* Player;

};
