// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AlbertosToPlayerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UAlbertosToPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAlbertosToPlayerComponent();

protected:
	virtual void BeginPlay() override;

public:	

	void CallAlbertosToThePlayer(FVector PlayerLoc);
	void ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer = true);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
		float MaxSpeedWhenMovingTowardsPlayer = 3000.f;
	// Teleports albertos to the player when the player is far away.
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
		float MaxDistanceToPlayer = 4000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
		float TeleportToPlayerRadius = 3000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Call Albertos To Player")
		TObjectPtr<USoundBase>CallAlbertosSound;

	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosPawn> AlbertosPawn;
	UPROPERTY(Transient)
		TObjectPtr<class AAlbertosAIController> AlbertosAI;

	float OriginalMoveSpeed;

	/// <summary>
	/// Check if the distance to the player is greater than MaxDistanceToPlayer, if so, teleport Alberto near the player.
	/// </summary>
	/// <returns>True - if albertos is further from the player then MaxDistanceToPlayer <br/>
	/// False - otherwise</returns>
	bool TeleportAlbertosToPlayer(FVector& PlayerLoc);

	void SetInitialAlbertosVariables();
};
