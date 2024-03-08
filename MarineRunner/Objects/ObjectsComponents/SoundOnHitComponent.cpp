// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/ObjectsComponents/SoundOnHitComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Interfaces/TakeInterface.h"

// Sets default values for this component's properties
USoundOnHitComponent::USoundOnHitComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void USoundOnHitComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnActorHit.AddDynamic(this, &USoundOnHitComponent::OnHitPlaySound);
}

void USoundOnHitComponent::OnHitPlaySound(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!CanSpawnHitOnGroundSound(OtherActor))
		return;

	SpawnedHitGroundSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitGroundSound, GetOwner()->GetActorLocation());
}

bool USoundOnHitComponent::CanSpawnHitOnGroundSound(TObjectPtr<AActor> HitActor)
{
	if (!IsValid(HitGroundSound) || !IsValid(GetOwner()))
		return false;

	if (FMath::IsNearlyEqual(GetOwner()->GetVelocity().Length(), 0.f, VelocityLengthErrorTolerance))
		return false;

	ITakeInterface* TakeInterface = Cast<ITakeInterface>(HitActor); // Does not play sound if the hit object is pickable due to the overlap of the sound playback function when hit
	if (TakeInterface)
		return false;

	if (IsValid(SpawnedHitGroundSound))
	{
		if (SpawnedHitGroundSound->IsActive())
			return false;
	}

	return true;
}
