// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Interfaces/DamageInterface.h"

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
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	TEnumAsByte<EWeaponType> WeaponType = EWT_Knife;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	float QuickAttackTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack")
	float CanAttackAgainTime = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Spawn Knife")
	FSoftClassPath WeaponToSpawn;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Spawn Knife")
	FName SocketNameToAttach = FName(TEXT("Weapon_L"));

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Raycast")
	float MaxDistanceToDamage = 400.f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Raycast")
	FVector SweepBoxSize = FVector(25.f);

	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TSubclassOf<UCameraShakeBase> QuickAttackCameraShake = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<USoundBase> QuickAttackSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<UAnimationAsset> QuickAttackAnim = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects")
	TObjectPtr<USoundBase> DefaultHitSound = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects|Hit Decal")
	TObjectPtr<UMaterialInstance> WeaponHoleDecalMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects|Hit Decal")
	FVector WeaponHoleDecalSize = FVector(40.f, 20.f, 5.f);
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects|Hit Decal")
	float HitDecalFadeOutDelay = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Quick Attack|Effects|Hit Decal")
	float HitDecalFadeOutDuration = 2.f;

	UPROPERTY(Transient)
	bool bCanQuickAttack = true;
	UPROPERTY(Transient)
	FTimerHandle QuickAttackHandle = FTimerHandle();
	UPROPERTY(Transient)
	FTimerHandle CanQuickAttackAgainHandle = FTimerHandle();
	UPROPERTY(Transient)
	TObjectPtr<AActor> SpawnedQuickWeapon = nullptr;

	void QuickAttackFinished();
	FORCEINLINE void SetCanQuickAttackAgain() { bCanQuickAttack = true; }

	bool CanPlayerQuickAttack();
	void SpawnQuickAttackWeapon();
	void QuickAttackEffects();
	void QuickAttackHit();
	void HitWithoutDamageInterface(const FHitResult& HitResultFromAttack);
	void SpawnHitDecal(const FHitResult& HitResultFromAttack);

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player = nullptr;
	UPROPERTY(Transient)
	TObjectPtr<APlayerController> PlayerController = nullptr;
};
