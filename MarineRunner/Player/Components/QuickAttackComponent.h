// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuickAttackComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UQuickAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuickAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	void QuickAttack();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	float Damage = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	float DamageInterfaceImpulseForce = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	float DefaultHitImpulseForce = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Spawn Knife")
	FSoftClassPath WeaponToSpawn;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Spawn Knife")
	FName SocketNameToAttach = FName();

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Raycast")
	float MaxDistanceToDamage = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Raycast")
	FVector SweepBoxSize = FVector(1.f);

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TSubclassOf<UCameraShakeBase> QuickAttackCameraShake = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<USoundBase> QuickAttackSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<UAnimationAsset> QuickAttackAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<USoundBase> DefaultHitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<UMaterialInstance> WeaponHoleDecalMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	FVector WeaponHoleDecalSize = FVector(1.f);

	UPROPERTY(Transient)
	bool bCanQuickAttack = true;
	UPROPERTY(Transient)
	FTimerHandle QuickAttackHandle = FTimerHandle();
	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedQuickWeapon = nullptr;

	void SpawnQuickAttackWeapon();
	void QuickAttackEffects();
	void QuickAttackFinished();
	void HitWithoutDamageInterface(const FHitResult& HitResultFromAttack);
	void SpawnHitDecal(const FHitResult& HitResultFromAttack);

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
};
