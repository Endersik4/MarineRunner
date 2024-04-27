// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Weapon/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"
#include "MarineRunner/Player/Components/ArmsSwayComponent.h"
#include "MarineRunner/Player/Inventory/WeaponInventoryComponent.h"
#include "MarineRunner/Player/Widgets/HUDWidget.h"
#include "MarineRunner/Objects/Interactive/PickableItem.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Skeletal Mesh"));
	RootComponent = WeaponSkeletalMesh;
	WeaponSkeletalMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::PickUpWeaponItem(AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad)
{
	if (!IsValid(PlayerWhoTook))
		return;

	Player = PlayerWhoTook;

	AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNameToAttachWeapon);

	if (IsValid(Player->GetArmsSkeletalMesh()))
	{
		Player->GetArmsSkeletalMesh()->SetForceRefPose(false);
	}
	if (IsValid(Player->GetWeaponInventoryComponent()))
	{
		Player->GetWeaponInventoryComponent()->SetWeaponFromInventory(nullptr);
		Player->GetWeaponInventoryComponent()->AddNewWeaponToStorage(this);
	}
	if (IsValid(Player->GetWeaponHandlerComponent()))
	{
		Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);
		Player->GetWeaponHandlerComponent()->HideCurrentHoldingWeapon();
		Player->GetWeaponHandlerComponent()->SetWeapon(this);
	}
	if (IsValid(Player->GetHudWidget()))
	{
		Player->GetHudWidget()->ShowWeaponOnHud();
	}

	PlayGivenWeaponWithArmsAnimation(bWasOnceItemTaken ? DrawWeapon_Anim : FirstTimePickedUpWeapon_Anim);

	FTimerHandle CanChangeWeaponHandle;
	FTimerDelegate SetCanChangeWeaponDelegate = FTimerDelegate::CreateUObject(Player->GetWeaponHandlerComponent(), &UWeaponHandlerComponent::SetCanChangeWeapon, true);
	GetWorld()->GetTimerManager().SetTimer(CanChangeWeaponHandle, SetCanChangeWeaponDelegate, (bWasOnceItemTaken ? DrawWeapon_Anim : FirstTimePickedUpWeapon_Anim).ArmsActionAnim->GetPlayLength(), false);
}

void AWeaponBase::DrawWeapon()
{
	if (!IsValid(Player))
		return;

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	PlayGivenWeaponWithArmsAnimation(DrawWeapon_Anim);

	if (IsValid(Player->GetWeaponHandlerComponent()))
		Player->GetWeaponHandlerComponent()->SetWeapon(this);
	if (IsValid(Player->GetHudWidget()))
		Player->GetHudWidget()->ShowWeaponOnHud();

	if (IsValid(DrawWeapon_Sound))
		UGameplayStatics::PlaySound2D(GetWorld(), DrawWeapon_Sound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Draw Weapon Sound is nullptr in Weapon %s"), *GetActorLabel());

	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(true);

	FTimerHandle CanChangeWeaponHandle;
	FTimerDelegate SetCanChangeWeaponDelegate = FTimerDelegate::CreateUObject(Player->GetWeaponHandlerComponent(), &UWeaponHandlerComponent::SetCanChangeWeapon, true);
	GetWorld()->GetTimerManager().SetTimer(CanChangeWeaponHandle, SetCanChangeWeaponDelegate, DrawWeapon_Anim.ArmsActionAnim->GetPlayLength(), false);
}

void AWeaponBase::PutAwayWeapon()
{
	if (!IsValid(Player))
		return;

	PlayGivenWeaponWithArmsAnimation(PutAwayWeapon_Anim);

	if (IsValid(Player->GetWeaponHandlerComponent()))
		Player->GetWeaponHandlerComponent()->SetWeapon(nullptr);
	if (IsValid(Player->GetHudWidget()))
		Player->GetHudWidget()->ShowWeaponOnHud(false);

	if (IsValid(PutAwayWeapon_Sound))
		UGameplayStatics::PlaySound2D(GetWorld(), PutAwayWeapon_Sound);
	else
		UE_LOG(LogTemp, Warning, TEXT("PutAwayWeapon_Sound is nullptr in Weapon %s"), *GetActorLabel());

	FTimerHandle HideWeaponHandle;
	GetWorld()->GetTimerManager().SetTimer(HideWeaponHandle, this, &AWeaponBase::HideWeapon, PutAwayWeapon_Anim.ArmsActionAnim->GetPlayLength(), false);
}

void AWeaponBase::HideWeapon()
{
	if (!IsValid(Player))
		return;

	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	WeaponSkeletalMesh->Stop();

	Player->GetArmsSkeletalMesh()->Stop();

	// Back to T-Pose if there is no gun to draw after dropping current gun
	if (Player->GetWeaponInventoryComponent()->GetCurrentAmountOfWeapons() == 1 && bDropWeaponAfterPutAway)
	{
		Player->GetArmsSkeletalMesh()->SetForceRefPose(true);
	}
	else
	{
		Player->GetWeaponHandlerComponent()->DrawNewEquipedWeapon();
	}

	DropWeapon();
}

void AWeaponBase::DropWeapon()
{
	if (!bDropWeaponAfterPutAway)
		return;

	if (!IsValid(ItemToSpawnAfterDrop))
		return;

	Player->GetWeaponInventoryComponent()->RemoveWeaponFromStorage(this);

	if (IsValid(DropWeaponSound))
		UGameplayStatics::PlaySound2D(GetWorld(), DropWeaponSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("DropWeaponSound is nullptr in Weapon%s"), *GetActorLabel());

	const FVector& LocationToSpawnWeaponItem = Player->GetCameraLocation() + Player->GetCamera()->GetForwardVector() * DistanceToDropWeapon;
	const FTransform& WeaponItemTransform = FTransform(FRotator(0.f), LocationToSpawnWeaponItem);
	SpawnedDroppedWeaponItem = GetWorld()->SpawnActorDeferred<APickupItem>(ItemToSpawnAfterDrop, WeaponItemTransform);

	if (IsValid(SpawnedDroppedWeaponItem))
	{
		SpawnedDroppedWeaponItem->SetSavedValue(0);
		SpawnedDroppedWeaponItem->SetItemWasOnceTaken(true);
		SpawnedDroppedWeaponItem->SaveItemIfSpawnedRunTime();
		SpawnedDroppedWeaponItem->FinishSpawning(WeaponItemTransform);
	}

	FItemStruct* WeaponInformation = Player->GetInventoryComponent()->GetItemInformationFromDataTable(SpawnedDroppedWeaponItem->GetItemRowName());
	if (WeaponInformation)
		Player->GetInventoryComponent()->Inventory_Items.Remove(*WeaponInformation);

	Player->UpdateAlbertosInventory(true);
}

void AWeaponBase::PrimaryAction()
{
	
}

void AWeaponBase::ReleasedPrimaryAction()
{
}

void AWeaponBase::SecondaryAction()
{
}

void AWeaponBase::ReleasedSecondaryAction()
{
}

void AWeaponBase::TertiaryAction(float Value)
{
}

void AWeaponBase::ActionFromKey_One()
{
}

void AWeaponBase::UpdateWeaponHudInformation(bool bUpdateStoredAmmoText, bool bUpdateWeaponImage)
{
}

void AWeaponBase::PlayGivenWeaponWithArmsAnimation(const FWeaponAnimation& AnimToPlay) const
{
	if (AnimToPlay.WeaponActionAnim)
	{
		WeaponSkeletalMesh->PlayAnimation(AnimToPlay.WeaponActionAnim, false);
	}

	if (AnimToPlay.ArmsActionAnim && IsValid(Player))
	{
		Player->GetArmsSkeletalMesh()->PlayAnimation(AnimToPlay.ArmsActionAnim, false);
	}
}
