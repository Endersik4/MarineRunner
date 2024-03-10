// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/Components/AlbertosToPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"

UAlbertosToPlayerComponent::UAlbertosToPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAlbertosToPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	SetInitialAlbertosVariables();
}

void UAlbertosToPlayerComponent::CallAlbertosToThePlayer(FVector PlayerLoc)
{
	if (!IsValid(AlbertosAI) || !IsValid(AlbertosPawn))
		return;

	if (IsValid(CallAlbertosSound))
		UGameplayStatics::PlaySound2D(GetWorld(), CallAlbertosSound);

	if (TeleportAlbertosToPlayer(PlayerLoc))
		return;

	PlayerLoc.Z = AlbertosPawn->GetActorLocation().Z;
	AlbertosAI->CallAlbertosToThePlayer(PlayerLoc);

	ChangeMaxSpeedOfFloatingMovement(true);
}

// if player is further away from TeleportToPlayerRadius then teleport albertos to location near player
bool UAlbertosToPlayerComponent::TeleportAlbertosToPlayer(FVector& PlayerLoc)
{
	if (FVector::Distance(PlayerLoc, AlbertosPawn->GetActorLocation()) < MaxDistanceToPlayer)
		return false;

	FNavLocation RandomTeleportLocation;
	bool bFoundLoc = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(PlayerLoc, TeleportToPlayerRadius, RandomTeleportLocation);
	if (!bFoundLoc)
		return false;

	AlbertosPawn->SetActorLocation(RandomTeleportLocation.Location);
	return true;
}

void UAlbertosToPlayerComponent::ChangeMaxSpeedOfFloatingMovement(bool bTowardsPlayer)
{
	if (!IsValid(AlbertosPawn))
		return;

	if (bTowardsPlayer) AlbertosPawn->SetFloatingMovementMaxSpeed(MaxSpeedWhenMovingTowardsPlayer);
	else AlbertosPawn->SetFloatingMovementMaxSpeed(OriginalMoveSpeed);
}

void UAlbertosToPlayerComponent::SetInitialAlbertosVariables()
{
	AlbertosPawn = Cast<AAlbertosPawn>(GetOwner());
	if (!IsValid(AlbertosPawn))
		return;

	OriginalMoveSpeed = AlbertosPawn->GetFloatingMovementMaxSpeed();
	AlbertosAI = Cast<AAlbertosAIController>(AlbertosPawn->GetController());
}