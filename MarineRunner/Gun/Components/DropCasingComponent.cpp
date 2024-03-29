// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Gun/Components/DropCasingComponent.h"

#include "MarineRunner/Gun/Gun.h"

UDropCasingComponent::UDropCasingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UDropCasingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Gun is nullptr in GunControlsComponenet!")))
	{
		Gun = Cast<AGun>(GetOwner());
	}
}

void UDropCasingComponent::StartTimerToDropCasing()
{
	if (!bCasingEjection)
		return;

	GetWorld()->GetTimerManager().SetTimer(DropCasingHandle, this, &UDropCasingComponent::DropCasing, SpawnCasingAfterTime, false);
}

void UDropCasingComponent::DropCasing()
{
	if (!IsValid(DropBulletClass) || !IsValid(Gun))
		return;
	if (!IsValid(Gun->GetGunSkeletalMesh()))
		return;

	FRotator DropBulletRotation = Gun->GetActorRotation();
	if (bShouldRandomizeRotationOfCasing)
	{
		DropBulletRotation.Roll += FMath::FRandRange(RandomCasingRotation_Roll.GetLowerBoundValue(), RandomCasingRotation_Roll.GetUpperBoundValue());
		DropBulletRotation.Pitch += FMath::FRandRange(RandomCasingRotation_Pitch.GetLowerBoundValue(), RandomCasingRotation_Pitch.GetUpperBoundValue());
		DropBulletRotation.Yaw += FMath::FRandRange(RandomCasingRotation_Yaw.GetLowerBoundValue(), RandomCasingRotation_Yaw.GetUpperBoundValue());
	}

	TObjectPtr<AActor> DropBullet = GetWorld()->SpawnActor<AActor>(DropBulletClass, Gun->GetGunSkeletalMesh()->GetSocketLocation(SocketNameLocationToSpawnCasing), DropBulletRotation);
	if (!IsValid(DropBullet))
		return;

	DropBullet->AttachToActor(Gun, FAttachmentTransformRules::KeepWorldTransform);
}

void UDropCasingComponent::CancelDropCasingTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(DropCasingHandle);
}