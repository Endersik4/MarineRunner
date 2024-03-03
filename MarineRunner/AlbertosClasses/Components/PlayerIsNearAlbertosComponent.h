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

	void OpenAlbertosDoor(bool bOpenDoor);

	FORCEINLINE void SetPlayerPawn(APawn* NewPlayerPawn) { Player = NewPlayerPawn; }
	FORCEINLINE void SetRotateAlbertosTowardPlayer(bool bRotate) { bRotateAlbertosTowardPlayer = bRotate; }
private:

	UPROPERTY(EditAnywhere, Category = "Setting up Player Is Near")
		bool bIgnorePlayer = false;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float ActiveAlbertosRadius = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float TimeToCheckIfPlayerIsNear = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float SpeedToRotateTowardsPlayer = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float ToleranceToRotateAlbertos = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		UAnimMontage* AlbertosOpenAnimationMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		UAnimMontage* AlbertosCloseAnimationMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		FName OpenDoorSoundSocketName = FName(TEXT("Wysuwak"));
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		USoundBase* OpenDoorSound;

	UPROPERTY(EditAnywhere, Category = "Setting up Media Player")
		class UMediaPlayer* AlbertosMediaPlayer;
	UPROPERTY(EditAnywhere, Category = "Setting up Media Player")
		class UMediaSource* AlbertosMediaSource;

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
