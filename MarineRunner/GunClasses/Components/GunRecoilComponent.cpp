// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/GunClasses/Components/GunRecoilComponent.h"

UGunRecoilComponent::UGunRecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGunRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UGunRecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

