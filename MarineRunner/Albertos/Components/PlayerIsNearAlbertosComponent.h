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

	void OpenAlbertosDoor(const bool bOpenDoor);

	FORCEINLINE void SetPlayerPawn(TObjectPtr<APawn> NewPlayerPawn) { Player = NewPlayerPawn; }
	FORCEINLINE void SetRotateAlbertosTowardPlayer(bool bRotate) { bRotateAlbertosTowardPlayer = bRotate; }
private:

	UPROPERTY(EditAnywhere, Category = "Setting up Player Is Near")
		bool bIgnorePlayer = false;
	// if player is further then MaxPlayerDistanceToActiveAlbertos, Albertos goes back to wendering, otherwise = stops and rotates toward player
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float MaxPlayerDistanceToActiveAlbertos = 600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float TimeToCheckIfPlayerIsNear = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float SpeedToRotateTowardsPlayer = 8.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Player Is Near", meta = (EditCondition = "!bIgnorePlayer"))
		float ToleranceToRotateAlbertos = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		TObjectPtr<UAnimMontage> AlbertosOpenAnimationMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		TObjectPtr<UAnimMontage> AlbertosCloseAnimationMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		FName OpenDoorSoundSocketName = FName(TEXT("Wysuwak"));
	UPROPERTY(EditDefaultsOnly, Category = "Setting up Albertos|Open/Close Animation")
		TObjectPtr<USoundBase> OpenDoorSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setting up Media Player")
		TObjectPtr<class UMediaPlayer> AlbertosMediaPlayer = nullptr;
	UPROPERTY(EditAnywhere, Category = "Setting up Media Player")
		TObjectPtr<class UMediaSource> AlbertosMediaSource = nullptr;

	UPROPERTY(Transient)
		bool bIsFrontDoorOpen = false;

	// When Players is near Albertos
	UPROPERTY(Transient)
		bool bPlayerIsClose = false;
	UPROPERTY(Transient)
		bool bRotateAlbertosTowardPlayer = false;
	FTimerHandle PlayerIsNearHandle;
	void CheckIfThePlayerIsNear();
	void GoBackToWendering();
	void PlayerIsCloseToAlbertos();
	void RotateAlbertosTowardsPlayer(float Delta);

	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosOwner = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosAIController> AlbertosAIController = nullptr;
	UPROPERTY(Transient)
		TObjectPtr<APawn> Player = nullptr;

	void SetUpAlbertosAndPlayerVariables();
};
