// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Weapon/Throwable/IdentityDisc.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"

AIdentityDisc::AIdentityDisc()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AIdentityDisc::BeginPlay()
{
	Super::BeginPlay();
	
}

void AIdentityDisc::PickUpWeaponItem(AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad)
{
	Super::PickUpWeaponItem(PlayerWhoTook, bWasOnceItemTaken, ValueToLoad);
}

void AIdentityDisc::DrawWeapon()
{
	Super::DrawWeapon();
}

void AIdentityDisc::PutAwayWeapon()
{
	Super::PutAwayWeapon();
}

void AIdentityDisc::HideWeapon()
{
	Super::HideWeapon();
}

void AIdentityDisc::DropWeapon()
{
	Super::DropWeapon();
}

void AIdentityDisc::PrimaryAction()
{
	Super::PrimaryAction();
	PlayGivenWeaponWithArmsAnimation(ThrowDiscAnim);

	FTimerHandle HideGunHandle;
	GetWorld()->GetTimerManager().SetTimer(HideGunHandle, this, &AIdentityDisc::ThrowDisc, TimeToThrowDisc, false);
}

void AIdentityDisc::ThrowDisc()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	WeaponSkeletalMesh->SetCollisionProfileName(FName("PhysicsActor"));
	WeaponSkeletalMesh->SetSimulatePhysics(true);
	WeaponSkeletalMesh->AddImpulse(Player->GetCamera()->GetForwardVector() * 5000.f);
}

void AIdentityDisc::ReleasedPrimaryAction()
{
	Super::ReleasedPrimaryAction();

}

void AIdentityDisc::SecondaryAction()
{
	Super::SecondaryAction();

}

void AIdentityDisc::ReleasedSecondaryAction()
{
	Super::ReleasedSecondaryAction();

}

void AIdentityDisc::TertiaryAction(float Value)
{
	Super::TertiaryAction(Value);

}

void AIdentityDisc::ActionFromKey_One()
{
	Super::ActionFromKey_One();

}

void AIdentityDisc::UpdateWeaponHudInformation()
{
	Super::UpdateWeaponHudInformation();

}

int32 AIdentityDisc::GetIntValueToSave()
{
	return int32();
}

void AIdentityDisc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

