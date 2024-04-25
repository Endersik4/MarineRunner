// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAnimation
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		TObjectPtr<UAnimationAsset> WeaponActionAnim;
	UPROPERTY(EditAnywhere)
		TObjectPtr<UAnimationAsset> ArmsActionAnim;

	FWeaponAnimation()
	{
		WeaponActionAnim = nullptr;
		ArmsActionAnim = nullptr;
	}

	FORCEINLINE bool IsWeaponAnimationValid() const {
		return WeaponActionAnim && ArmsActionAnim;
	}
};

UCLASS()
class MARINERUNNER_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void PickUpWeaponItem(class AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad);

	virtual void DrawWeapon();
	virtual void PutAwayWeapon();
	virtual void HideWeapon();
	virtual void DropWeapon();

	virtual void PrimaryAction();
	virtual void ReleasedPrimaryAction();
	virtual void SecondaryAction();
	virtual void ReleasedSecondaryAction();
	virtual void TertiaryAction(float Value);

	virtual void ActionFromKey_One();

	virtual void UpdateWeaponHudInformation();

	FORCEINLINE FString GetPathToWeaponClass() { return GetClass()->GetClassPathName().ToString(); }
	FORCEINLINE virtual int32 GetIntValueToSave() { return int32(); }

	FORCEINLINE void SetDropWeapon(bool bDrop) { bDropWeaponAfterPutAway = bDrop; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class USkeletalMeshComponent> WeaponSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
		FName SocketNameToAttachWeapon = FName(TEXT("Weapon_R"));
		UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
		FWeaponAnimation FirstTimePickedUpWeapon_Anim = FWeaponAnimation();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Put Away")
		FWeaponAnimation PutAwayWeapon_Anim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Put Away")
		TObjectPtr<USoundBase> PutAwayWeapon_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Draw")
		FWeaponAnimation DrawWeapon_Anim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Draw")
		TObjectPtr<USoundBase> DrawWeapon_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
		TSubclassOf<class APickupItem> ItemToSpawnAfterDrop = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
		float DistanceToDropWeapon = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
		TObjectPtr<USoundBase> DropWeaponSound = nullptr;
	
	void PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const;

	bool bDropWeaponAfterPutAway;

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> Player;
		
};
