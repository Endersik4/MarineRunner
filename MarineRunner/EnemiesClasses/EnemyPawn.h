// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyPawn.generated.h"

UCLASS()
class MARINERUNNER_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	USkeletalMeshComponent* GetEnemySkeletalMesh() const { return EnemySkeletalMesh; }

	float GetHealth() const { return Health; }
	FColor GetBloodColor() const { return BloodColor; }
	int32 GetHowManyLocations() const { return HowManyLocations; }
	float GetWaitTimeShoot() const { return WaitTimeShoot; }
	float GetDetectPlayerTime() const { return DetectPlayerTime; }

	void ShootBullet() { Shoot(); }
	void SetIsDead(bool bNewDead);
	void SetHealth(float NewHealth) { Health = NewHealth; }

	void Reload();


	UFUNCTION(BlueprintImplementableEvent)
		void PlayAnimMontageInBlueprint();

	UFUNCTION(BlueprintImplementableEvent)
		void RestoreBonesToInitialRotation();

	//Had to get AnimInstance of AnimationBlueprint so in blueprint you are setting what bone you want to looking at the player
	UFUNCTION(BlueprintImplementableEvent)
		void FocusBonesOnPlayerWhenPlayerDetected();

	// Bone Rotation - Bone will be looking at the player when he is detected.
	// bLookStraight means whether Bone should look directly at the player's camera or with some margin
	UFUNCTION(BlueprintCallable)
		FRotator FocusBoneOnPlayer(FName BoneName, bool bLookStraight);

	UFUNCTION(BlueprintCallable)
		void SetCanShoot(bool bCan) { bCanShoot = bCan; }

	UFUNCTION(BlueprintPure)
		int32 GetCurrentMagazineCapacity() const { return MagazineCapacity; }

	UFUNCTION(BlueprintPure)
		bool GetIsReloading() const { return bIsReloading; }

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* EnemySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* EnemyFloatingMovement;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* CapsuleColl;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		int32 HowManyLocations = 5;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float WaitTimeShoot = 0.3f;

	//How long does it take to detect the player by enemy
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float DetectPlayerTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Enemy")
		float Health = 100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		float Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		int32 MagazineCapacity = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		UParticleSystem* ShootParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun|Particles")
		float ShootParticleScale;

	//Delay time after the enemy runs out of ammunition
	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float DelayTimeMagazine;
	//How fast ammo is moving forward. If Bullet has physics then this variable is Impulse Force
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoSpeed;
	//What distance should Ammo pass when bullet starts falling down
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoDistance;
	//How fast Ammo will falling down when AmmoDistance hit the number
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoFallingDown;
	UPROPERTY(EditAnywhere, Category = "Setting Up Bullet")
		float AmmoImpulseForce;

	//Bullet Type that will be fired from Gun
	UPROPERTY(EditDefaultsOnly, Category = "Setting Up Gun")
		TSubclassOf<AActor> BulletClass;

	//SOUNDS
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		class USoundBase* ShootingSound;

	//Particles
	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		FColor BloodColor;

	//Shooting
	bool bCanShoot = true;
	void Shoot();
	void PredictWhereToShoot();

	FVector CameraLocation;

	bool bIsDead;
	bool bIsReloading;

	//If enemy see the player then he will execute given functions
	void CheckIfEnemySeePlayer();

	//Reload
	FTimerHandle DelayEmptyMagazineHandle;
	void DelayAfterEmptyMagazine();

	//Bullet
	FTimerHandle ImpulseOnBulletHandle;
	void SpawnBullet();

	//MarinePawn
	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();

	//EnemyAIController
	class AEnemyAiController* EnemyAIController;
	void SetUpEnemyAIController();

	//Other
	int32 CopyOfMagazineCapacity;
};
