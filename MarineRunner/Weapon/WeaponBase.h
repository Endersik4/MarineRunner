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
	// Overridable native event for preparing weapon for the player that picked up weapon item. 
	virtual void PickUpWeaponItem(class AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad);

	// Overridable native event | activates this weapon to be the current one. 
	virtual void DrawWeapon();
	// Overridable native event | Deactivates this weapon. 
	virtual void PutAwayWeapon();
	// Overridable native event | After PutAway anim hides the weapon. 
	virtual void HideWeapon();
	// Overridable native event | Removes weapon from inventory, spawnes weapon item in front of the player. 
	virtual void DropWeapon();

	// Overridable native event | Calls Primary Action in Weapon e.g Shoot in Gun 
	virtual void PrimaryAction();
	virtual void ReleasedPrimaryAction();
	// Overridable native event | Calls Secondary Action in Weapon e.g ADS in Gun 
	virtual void SecondaryAction();
	virtual void ReleasedSecondaryAction();
	// Overridable native event | Calls Tertiary Action in Weapon e.g Zoom in Gun 
	virtual void TertiaryAction(float Value);

	// Overridable native event | Calls Key_One Action in Weapon e.g Reload in Gun  
	virtual void ActionFromKey_One();

	virtual void UpdateWeaponHudInformation(bool bUpdateStoredAmmoText = false, bool bUpdateWeaponImage = false);

	void PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const;

	FORCEINLINE FString GetPathToWeaponClass() { return GetClass()->GetClassPathName().ToString(); }
	FORCEINLINE virtual int32 GetIntValueToSave() { return int32(); }
	FORCEINLINE TObjectPtr<class USkeletalMeshComponent> GetWeaponSkeletalMesh() const { return WeaponSkeletalMesh; }
	float GetPutAwayWeaponAnimTime();

	FORCEINLINE void SetDropWeapon(bool bDrop) { bDropWeaponAfterPutAway = bDrop; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USkeletalMeshComponent> WeaponSkeletalMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings")
	FName SocketNameToAttachWeapon = FName(TEXT("Weapon_R"));

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Draw")
	FWeaponAnimation FirstTimePickedUpWeapon_Anim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Draw")
	FWeaponAnimation DrawWeapon_Anim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Draw")
	TObjectPtr<USoundBase> DrawWeapon_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Put Away")
	FWeaponAnimation PutAwayWeapon_Anim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Put Away")
	TObjectPtr<USoundBase> PutAwayWeapon_Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
	TSubclassOf<class APickupItem> ItemToSpawnAfterDrop = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
	float DistanceToDropWeapon = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Settings|Drop Weapon")
	TObjectPtr<USoundBase> DropWeaponSound = nullptr;

	bool bDropWeaponAfterPutAway;
	UPROPERTY(Transient)
	TObjectPtr<APickupItem> SpawnedDroppedWeaponItem;

	UPROPERTY(Transient)
	TObjectPtr<class AMarineCharacter> Player;
};
