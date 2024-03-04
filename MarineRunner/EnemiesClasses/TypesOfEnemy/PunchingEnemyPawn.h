// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
#include "PunchingEnemyPawn.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API APunchingEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

protected:
	APunchingEnemyPawn();

public:
	void PlayerDetected(bool bDetected, APawn* DetectedPawn = nullptr);

private:

	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Check if player is close")
		float PlayerCloseRaycastInterval = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Check if player is close")
		FName PlayerCloseRaycastSocketNameLocation = "PunchSocket";
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Check if player is close")
		float MaxPlayerCloseRaycastDistance = 100.f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		float Damage = 6.f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		float DamageImpulseForce = 50000.f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		float ApplyDamageTime = 0.1f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		float DamageSphereRadius = 20.f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		float CanDamageAgainTime = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		TObjectPtr<UAnimMontage> PunchAnimMontage;
	UPROPERTY(EditAnywhere, Category = "Punching Enemy Settings|Punch")
		TObjectPtr<USoundBase> PunchSound;

	FTimerHandle PlayerCloseForHitHandle;
	void IsPlayerCloseForHit();

	void StartPunchingPlayer();
	bool bIsPunching;

	FVector DamageSphereLocation;
	void ApplyDamageToPlayer();

	void DamageAnimFinished();

	UPROPERTY(Transient)
		TObjectPtr<APawn> DetectedPlayer;
};
