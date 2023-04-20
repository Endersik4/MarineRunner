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

	UFUNCTION(BlueprintPure)
		FVector GetCameraLocation() { return CameraLocation; }

	UFUNCTION(BlueprintCallable)
	void ShootBullet() { Shoot(); }

	void SetHealth(float NewHealth) { Health = NewHealth; }

	UFUNCTION(BlueprintImplementableEvent)
		void PlayAnimMontageInBlueprint();

	UFUNCTION(BlueprintPure)
		int32 GetCurrentMagazineCapacity() const {return MagazineCapacity; }

	UFUNCTION(BlueprintCallable)
		void SetCanShoot(bool bCan) { bCanShoot = bCan; }

	UPROPERTY(EditDefaultsOnly, Category = "Components", BlueprintReadWrite)
		USkeletalMeshComponent* EnemySkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		class UFloatingPawnMovement* EnemyFloatingMovement;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* CapsuleColl;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		FColor BloodColor;

	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		int32 HowManyLocations = 5;

	//Center Distance 
	UPROPERTY(EditDefaultsOnly, Category = "AI Setup")
		float DetectingDistance = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Settings up Enemy")
		float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
		float Damage;

	UPROPERTY(EditAnywhere, Category = "Setting Up Gun")
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

	bool bCanShoot = true;
	int32 CopyOfMagazineCapacity;
	void Shoot();

	FVector CameraLocation;

	FTimerHandle DelayEmptyMagazineHandle;
	void DelayAfterEmptyMagazine();

	FTimerHandle ImpulseOnBulletHandle;
	void SpawnBullet();

	class AMarineCharacter* MarinePawn;
	void SetUpMarinePawn();
};
