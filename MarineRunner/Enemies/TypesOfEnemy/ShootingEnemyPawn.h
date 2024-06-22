// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/Enemies/TypesOfEnemy/Base/EnemyBasePawn.h"
#include "ShootingEnemyPawn.generated.h"

/**
 *
 */
UCLASS()
class MARINERUNNER_API AShootingEnemyPawn : public AEnemyPawn
{
	GENERATED_BODY()

protected:
	AShootingEnemyPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

public:
	virtual void ApplyDamage(float NewDamage, float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius) override;

	FORCEINLINE virtual class AActor* GetFocusedActor() override { return FocusedActor; }

	UFUNCTION(BlueprintPure)
	float GetSpeedOfEnemyWhenIsRunningAway() const { return SpeedOfEnemyWhenIsRunningAway; }

	UFUNCTION(BlueprintImplementableEvent)
	void SetShouldRunningAwayInAnimBP();

	//Had to get AnimInstance of AnimationBlueprint so in blueprint you are setting what bone you want to looking at the player
	UFUNCTION(BlueprintImplementableEvent)
	void FocusBonesOnPlayerWhenPlayerDetected();

	// Bone Rotation - Bone will be looking at the player when he is detected.
	// bLookStraight means whether Bone should look directly at the player's camera or with some margin
	UFUNCTION(BlueprintCallable)
	const FRotator FocusBoneOnPlayer(const FName BoneName, const bool bLookStraight);

	void SawTheTarget(bool bSaw, TObjectPtr<AActor> SeenTarget = nullptr, bool bStartAttackingTheTarget = true);

	void ApplyEnemyDifficulty();

protected:

	virtual bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, TObjectPtr<AActor> BulletActor, float NewSphereRadius) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UEnemyGunComponent> EnemyGunComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
	float ShootTime = 1.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
	FFloatRange StartShootingRandomTimeRange = FFloatRange(1.f, 3.f);
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
	TObjectPtr<UAnimMontage> ShootAnimMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
	TObjectPtr<UAnimMontage> EnableShootAnimMontage = nullptr;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
	TObjectPtr<UAnimMontage> DisableShootAnimMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot")
	bool bAlertAboutShoot = false;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
	int32 AlertMaterialIndexToChange = 0;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
	FName AlertParameterNameToChange = FName();
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
	float BeginParameterChangeAfterShootTime = 0.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
	float FinishAlertParameterChangeTime = 0.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
	float EndValueForAlertParameter = 0.f;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Enemy Run Away")
	bool bCanEnemyRunAway = true;
	// if enemy has less hp then MaxEnemyHealthForRunAway then choose random if enemy should run away
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
	float MaxEnemyHealthForRunAway = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
	float SpeedOfEnemyWhenIsRunningAway = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
	int32 PercentForEnemyRunaway = 20;

	void ShouldRunAway();
	bool EnemyRunAway();

	// Shooting
	FTimerHandle StartShootingHandle;
	FTimerHandle ShootHandle;
	void StartShooting();
	void Shoot();
	void PlayPrepareToShootAnimation(bool bTargetWasDetected);

	UPROPERTY(Transient)
	float AlertTimeElapsed = 0.f;
	UPROPERTY(Transient)
	bool bStartAlert = false;
	void SetUpShootAlert();
	void ChangeParameterInAlertMaterial(float Delta);
	void ResetAlertMaterial();
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic>CurrentAlertMaterial = nullptr;

	UPROPERTY(Transient)
	float OriginalShootTime = 1.f;
	UPROPERTY(Transient)
	float OriginalMaxSpeed = 1000.f;

	//If enemy see the player then he will execute given functions
	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor = nullptr;
	bool bEnemyDetectedTarget = false;

	void SetEnemyKilledInAIController();
};
