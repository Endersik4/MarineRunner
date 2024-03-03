// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MarineRunner/EnemiesClasses/EnemyPawn.h"
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
		FRotator FocusBoneOnPlayer(FName BoneName, bool bLookStraight);

	void SawTheTarget(bool bSaw, AActor* SeenTarget = nullptr, bool bStartAttackingTheTarget = true);
protected:

	virtual bool KillEnemy(float NewImpulseForce, const FHitResult& NewHit, AActor* BulletActor, float NewSphereRadius) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
		class UEnemyGunComponent* EnemyGunComponent;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		float TimeToStartShooting = 1.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		float ShootTime = 1.f;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot")
		FFloatRange StartShootingRandomTimeRange = FFloatRange(1.f, 3.f);
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
		UAnimMontage* ShootAnimMontage;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
		UAnimMontage* EnableShootAnimMontage;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Shoot|Animations")
		UAnimMontage* DisableShootAnimMontage;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot")
		bool bAlertAboutShoot = false;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
		int32 AlertMaterialIndexToChange;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
		FName AlertParameterNameToChange;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
		float BeginParameterChangeAfterShootTime;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
		float FinishAlertParameterChangeTime;
	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Alert About Shoot", meta = (EditCondition = "bAlertAboutShoot", EditConditionHides))
		float EndValueForAlertParameter;

	UPROPERTY(EditAnywhere, Category = "Setting Enemy|Enemy Run Away")
		bool bCanEnemyRunAway = true;
	// if enemy has less hp then MaxEnemyHealthForRunAway then choose random if enemy should run away
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float MaxEnemyHealthForRunAway = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float SpeedOfEnemyWhenIsRunningAway = 1600.f;
	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy|Enemy Run Away", meta = (EditCondition = "bCanEnemyRunAway", EditConditionHides))
		float PercentForEnemyRunaway = 20.f;

	void ShouldRunAway();
	bool EnemyRunAway();

	// Shooting
	FTimerHandle StartShootingHandle;
	FTimerHandle ShootHandle;
	void StartShooting();
	void Shoot();
	void PlayPrepareToShootAnimation(bool bTargetWasDetected);

	// Alert about shoot
	float AlertTimeElapsed;
	bool bStartAlert;
	void SetUpShootAlert();
	void ChangeParameterInAlertMaterial(float Delta);
	void ResetAlertMaterial();
	UMaterialInstanceDynamic* CurrentAlertMaterial;

	//If enemy see the player then he will execute given functions
	AActor* FocusedActor;
	bool bEnemyDetectedTarget;

	void SetEnemyKilledInAIController();

};
