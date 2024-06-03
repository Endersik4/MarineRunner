// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/AlbertosAIController.h"
#include "MarineRunner/Albertos/Components/PlayerIsNearAlbertosComponent.h"

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
	if (!IsValid(AlbertosAIController) || !IsValid(AlbertosPawn))
		return;

	if (IsValid(CallAlbertosSound))
		UGameplayStatics::PlaySound2D(GetWorld(), CallAlbertosSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Call Albertos Sound is nullptr in AlbertosToPlayer Component"));

	TeleportAlbertosToPlayer(PlayerLoc);

	PlayerLoc.Z = AlbertosPawn->GetActorLocation().Z;
	AlbertosAIController->CallAlbertosToThePlayer(PlayerLoc);

	ChangeMaxSpeedOfFloatingMovement(true);
}

// if player is further away from TeleportToPlayerRadius then teleport albertos to location near player
void UAlbertosToPlayerComponent::TeleportAlbertosToPlayer(const FVector& PlayerLoc)
{
	TObjectPtr<UNavigationSystemV1> CurrentNavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!IsValid(CurrentNavigationSystem))
		return;

	// if albertos can go to the player then check if the distance
	float PlayerToAlbertosPathLength = 0.f;
	const ENavigationQueryResult::Type bFoundPathFromPlayerToAlbertos = CurrentNavigationSystem->GetPathLength(PlayerLoc, AlbertosPawn->GetActorLocation(), PlayerToAlbertosPathLength);
	if (bFoundPathFromPlayerToAlbertos != ENavigationQueryResult::Fail)
	{
		const float& DistanceToPlayer = FVector::Distance(PlayerLoc, AlbertosPawn->GetActorLocation());
		if (DistanceToPlayer < MaxDistanceToPlayer)
			return;
	}

	FNavLocation RandomTeleportLocation;
	const bool& bCanReachRandomPoint = CurrentNavigationSystem->GetRandomReachablePointInRadius(PlayerLoc, TeleportToPlayerRadius, RandomTeleportLocation);
	if (!bCanReachRandomPoint)
		return;

	AlbertosPawn->SetActorLocation(RandomTeleportLocation.Location);
}

void UAlbertosToPlayerComponent::ChangeMaxSpeedOfFloatingMovement(const bool bTowardsPlayer)
{
	if (!IsValid(AlbertosPawn))
		return;

	if (bTowardsPlayer)
	{
		AlbertosPawn->SetFloatingMovementMaxSpeed(MaxSpeedWhenMovingTowardsPlayer);
	}
	else
	{
		AlbertosPawn->SetFloatingMovementMaxSpeed(OriginalMoveSpeed);
	}
}

void UAlbertosToPlayerComponent::SetInitialAlbertosVariables()
{
	if (!ensureMsgf(IsValid(GetOwner()), TEXT("Albertos Pawn is nullptr in CraftItemAlbertosComponent!")))
		return;

	AlbertosPawn = Cast<AAlbertosPawn>(GetOwner());
	if (!IsValid(AlbertosPawn))
		return;

	OriginalMoveSpeed = AlbertosPawn->GetFloatingMovementMaxSpeed();

	if (!ensureMsgf(IsValid(AlbertosPawn->GetController()), TEXT("Albertos Controller is nullptr in CraftItemAlbertosComponent!")))
		return;

	AlbertosAIController = Cast<AAlbertosAIController>(AlbertosPawn->GetController());
}