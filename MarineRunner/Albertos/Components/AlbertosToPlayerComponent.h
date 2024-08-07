// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlbertosToPlayerComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UAlbertosToPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAlbertosToPlayerComponent();

protected:
	virtual void BeginPlay() override;

public:

	void CallAlbertosToThePlayer(FVector PlayerLoc);
	void ChangeMaxSpeedOfFloatingMovement(const bool bTowardsPlayer = true);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
	float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	// Teleports albertos to the player when the player is far away.
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
	float MaxDistanceToPlayer = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
	float TeleportToPlayerRadius = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player|Effects")
	TObjectPtr<USoundBase>CallAlbertosSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player|Albertos Icon")
	float AlbertosIconWidgetSpawnTime = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player|Albertos Icon")
	TSubclassOf<class UAlbertosIcon> AlbertosIconWidget = nullptr;

	UPROPERTY(Transient)
	float OriginalMoveSpeed = 0.f;

	/// <summary>
	/// Check if the distance to the player is greater than MaxDistanceToPlayer, if so, teleport Alberto near the player.
	/// </summary>
	void TeleportAlbertosToPlayer(const FVector& PlayerLoc);

	void SetInitialAlbertosVariables();

	void SpawnAlbertosIconWidgetOnPlayer();
	void DestroyAlbertosIconWidget();
	UPROPERTY(Transient)
	bool bAlbertosIconSpawned = false;
	UPROPERTY(Transient)
	TObjectPtr<class UAlbertosIcon> SpawnedAlbertosIconWidget = nullptr;
	UPROPERTY(Transient)
	FTimerHandle VisibleAlbertosIconHandle = FTimerHandle();

	UPROPERTY(Transient)
	TObjectPtr<class AAlbertosPawn> AlbertosPawn = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<class AAlbertosAIController> AlbertosAIController = nullptr;
};
