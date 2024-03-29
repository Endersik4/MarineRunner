// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/Components/AlbertosToPlayerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"

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

	if (TeleportAlbertosToPlayer(PlayerLoc))
		return;

	PlayerLoc.Z = AlbertosPawn->GetActorLocation().Z;
	AlbertosAIController->CallAlbertosToThePlayer(PlayerLoc);

	ChangeMaxSpeedOfFloatingMovement(true);
}

// if player is further away from TeleportToPlayerRadius then teleport albertos to location near player
bool UAlbertosToPlayerComponent::TeleportAlbertosToPlayer(const FVector& PlayerLoc)
{
	const float& DistanceToPlayer = FVector::Distance(PlayerLoc, AlbertosPawn->GetActorLocation());
	if (DistanceToPlayer < MaxDistanceToPlayer)
		return false;

	if (!IsValid(UNavigationSystemV1::GetCurrent(GetWorld())))
		return false;

	FNavLocation RandomTeleportLocation;
	const bool& bFoundLoc = UNavigationSystemV1::GetCurrent(GetWorld())->GetRandomReachablePointInRadius(PlayerLoc, TeleportToPlayerRadius, RandomTeleportLocation);
	if (!bFoundLoc)
		return false;

	AlbertosPawn->SetActorLocation(RandomTeleportLocation.Location);

	return true;
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