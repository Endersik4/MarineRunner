// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/ObjectsComponents/SoundOnHitComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

#include "MarineRunner/Interfaces/TakeInterface.h"

// Sets default values for this component's properties
USoundOnHitComponent::USoundOnHitComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void USoundOnHitComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->OnActorHit.AddDynamic(this, &USoundOnHitComponent::OnHitPlaySound);
}

// Called every frame
void USoundOnHitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USoundOnHitComponent::OnHitPlaySound(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HitGroundSound) 
		return;

	if (FMath::IsNearlyEqual(GetOwner()->GetVelocity().Length(), 0.f, VelocityLengthErrorTolerance))
		return;

	ITakeInterface* TakeInterface = Cast<ITakeInterface>(Hit.GetActor());
	if (TakeInterface) 
		return;

	if (IsValid(SpawnedHitGroundSound) == true)
	{
		if (SpawnedHitGroundSound->IsActive())
			return;
	}

	if (IsValid(SpawnedHitGroundSound) == true && IsValid(HitActor) == true)
	{
		if (SpawnedHitGroundSound->IsActive() && HitActor == OtherActor)
			return;
	}

	SpawnedHitGroundSound = UGameplayStatics::SpawnSoundAtLocation(GetWorld(), HitGroundSound, GetOwner()->GetActorLocation());
	HitActor = OtherActor;
}

