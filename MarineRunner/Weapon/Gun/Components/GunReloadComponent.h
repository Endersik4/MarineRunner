// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MarineRunner/Weapon/Gun/Gun.h"

#include "GunReloadComponent.generated.h"

UENUM(BlueprintType)
enum EReloadType
{
	ERT_BeginReload UMETA(DisplayName = "Begin Reload"),
	ERT_EndReload UMETA(DisplayName = "End Reload"),
	ERT_BeginEndReload UMETA(DisplayName = "Begin End Reload"),
	ERT_Reload UMETA(DisplayName = "Reload"),
};
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MARINERUNNER_API UGunReloadComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGunReloadComponent();

protected:
	virtual void BeginPlay() override;

public:
	void PrepareToReload();
	void CancelReload();
	bool CanShootWhileReloading();

	FORCEINLINE void SetMagazineCapacity(float NewMagazine) { MagazineCapacity = NewMagazine; }

	FORCEINLINE bool GetIsReloading() const { return bIsReloading; }
	FORCEINLINE bool GetReloadOneBullet() const { return bReloadOneBullet; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	FORCEINLINE int32 GetOriginalMagazineCapacity() const { return OriginalMagazineCapacity; }
private:

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	bool bReloadOneBullet = false;
	UPROPERTY(EditAnywhere, Category = "Reload")
	int32 MagazineCapacity = 10;

	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	FWeaponAnimation WeaponReloadAnim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	FWeaponAnimation WeaponReloadWithNoBulletsAnim = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (EditCondition = "bReloadOneBullet"))
	FWeaponAnimation WeaponReload_Begin = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (EditCondition = "bReloadOneBullet"))
	FWeaponAnimation WeaponReload_BeginEnd = FWeaponAnimation();
	UPROPERTY(EditDefaultsOnly, Category = "Reload", meta = (EditCondition = "bReloadOneBullet"))
	FWeaponAnimation WeaponReload_End = FWeaponAnimation();

	EReloadType CurrentReloadType = ERT_EndReload;
	EReloadType GetCurrentReloadTypeAccordingToSituation();

	UPROPERTY(Transient)
	float OriginalMagazineCapacity = 0.f;
	UPROPERTY(Transient)
	bool bIsReloading = false;

	FTimerHandle ReloadHandle;
	bool CanReload();
	void Reload();
	void ReloadMagazine(struct FItemStruct* AmmoFromInventory);
	const FWeaponAnimation& ReloadAnimAccordingToSituation();

	UPROPERTY(Transient)
	TObjectPtr<class AGun> Gun = nullptr;
};