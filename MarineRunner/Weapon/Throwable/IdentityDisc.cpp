// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Weapon/Throwable/IdentityDisc.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Interfaces/DamageInterface.h"

AIdentityDisc::AIdentityDisc()
{
	PrimaryActorTick.bCanEverTick = true;


}

void AIdentityDisc::BeginPlay()
{
	Super::BeginPlay();

	WeaponSkeletalMesh->OnComponentBeginOverlap.AddDynamic(this, &AIdentityDisc::OnBeginOverlap);
}

void AIdentityDisc::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DiscMovement(DeltaTime);
}

void AIdentityDisc::PickUpWeaponItem(AMarineCharacter* PlayerWhoTook, bool bWasOnceItemTaken, int32 ValueToLoad)
{
	Super::PickUpWeaponItem(PlayerWhoTook, bWasOnceItemTaken, ValueToLoad);
}

void AIdentityDisc::DrawWeapon()
{
	Super::DrawWeapon();
}

void AIdentityDisc::PutAwayWeapon(bool bImmediatelyHideWeapon, bool bRemoveWeaponFromCurrentOne)
{
	Super::PutAwayWeapon(bImmediatelyHideWeapon, bRemoveWeaponFromCurrentOne);
}

void AIdentityDisc::HideWeapon()
{
	Super::HideWeapon();
}

void AIdentityDisc::DropWeapon()
{
	Super::DropWeapon();

	Destroy();
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
	FHitResult HitResult;
	StartDiscThrowLocation = GetActorLocation();
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartDiscThrowLocation, StartDiscThrowLocation + (Player->GetCamera()->GetForwardVector() * MaxThrowDistance), ECC_Visibility);
	if (!bHit)
		return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EndDiscThrowLocation = HitResult.ImpactPoint;
	bStartTimer = true;

	DrawDebugBox(GetWorld(), StartDiscThrowLocation, FVector(10.f), FColor::Red, true);
	DrawDebugBox(GetWorld(), EndDiscThrowLocation, FVector(10.f), FColor::Green, true);
}

void AIdentityDisc::DiscMovement(float Delta)
{
	if (!bStartTimer)
		return;

	FVector NewLocation = FMath::VInterpConstantTo(GetActorLocation(), EndDiscThrowLocation, Delta, DiscSpeed);
	FVector NewScale = FMath::VInterpConstantTo(GetActorScale3D(), FVector(TargetScale), Delta,DiscSpeed);
	
	SetActorLocation(NewLocation);
	SetActorScale3D(NewScale);

	if (GetActorLocation().Equals(EndDiscThrowLocation, 50.f))
	{
		SetActorScale3D(FVector(TargetScale));
		bStartTimer = false;
	}
}

void AIdentityDisc::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor))
		return;

	IDamageInterface* DamageInterface = Cast<IDamageInterface>(OtherActor);
	if (!DamageInterface)
		return;

	DamageInterface->ApplyDamage(10000.f, 0.f, SweepResult, this);
}

void AIdentityDisc::ReleasedPrimaryAction()
{
	Super::ReleasedPrimaryAction();

}

void AIdentityDisc::SecondaryAction()
{
	Super::SecondaryAction();

	SetActorScale3D(FVector(0.1f));
	bStartTimer = false;

	AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNameToAttachWeapon);

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

void AIdentityDisc::UpdateWeaponHudInformation(bool bUpdateStoredAmmoText, bool bUpdateWeaponImage)
{
	Super::UpdateWeaponHudInformation();

}

int32 AIdentityDisc::GetIntValueToSave()
{
	return int32();
}

