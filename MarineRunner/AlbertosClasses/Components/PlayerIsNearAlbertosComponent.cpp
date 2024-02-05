// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/AlbertosClasses/AlbertosPawn.h"
#include "MarineRunner/AlbertosClasses/AlbertosAIController.h"
#include "MarineRunner/AlbertosClasses/Components/CraftItemAlbertosComponent.h"

UPlayerIsNearAlbertosComponent::UPlayerIsNearAlbertosComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerIsNearAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	AlbertosOwner = Cast<AAlbertosPawn>(GetOwner());
	AlbertosAI = Cast<AAlbertosAIController>(AlbertosOwner->GetController());
	Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	GetWorld()->GetTimerManager().SetTimer(PlayerIsNearHandle, this, &UPlayerIsNearAlbertosComponent::CheckIfThePlayerIsNear, TimeToCheckIfPlayerIsNear, true);
}

void UPlayerIsNearAlbertosComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RotateAlbertosTowardsPlayer(DeltaTime);
}

#pragma region ///////////////////////// The Player is Located Near Albertos /////////////////////////
void UPlayerIsNearAlbertosComponent::CheckIfThePlayerIsNear()
{
	if (IsValid(AlbertosAI) == false || bIgnorePlayer == false || IsValid(Player) == false)
		return;

	if (FVector::Distance(Player->GetActorLocation(), AlbertosOwner->GetActorLocation()) <= ActiveAlbertosRadius)
	{
		PlayerIsCloseToAlbertos();
	}
	else
		GoBackToWendering();
}

void UPlayerIsNearAlbertosComponent::PlayerIsCloseToAlbertos()
{
	if (bPlayerIsClose == true)
		return;

	AlbertosAI->SetCanMove(false);
	AlbertosAI->StopMovement();

	bRotateAlbertosTowardPlayer = true;

	bPlayerIsClose = true;
}

// if item is being crafted then wait till finish and then start wendering 
void UPlayerIsNearAlbertosComponent::GoBackToWendering()
{
	if (bPlayerIsClose == false || AlbertosOwner->GetCraftItemAlbertosComponent()->isCraftedItemValid() == true)
		return;

	AlbertosAI->SetCanMove(true);

	AlbertosOwner->SetInventoryVisibility(false);

	ToggleOpenDoor(false);

	bPlayerIsClose = false;
}

void UPlayerIsNearAlbertosComponent::ToggleOpenDoor(bool bOpenDoor)
{
	if (bIsFrontDoorOpen == bOpenDoor)
		return;

	AlbertosOwner->OpenFrontDoor(AlbertosOwner->GetAlbertosSkeletal(), bOpenDoor);
	bIsFrontDoorOpen = bOpenDoor;

	if (OpenDoorSound)
		UGameplayStatics::SpawnSoundAttached(OpenDoorSound, AlbertosOwner->GetAlbertosSkeletal(), OpenDoorSoundSocketName);
}

void UPlayerIsNearAlbertosComponent::RotateAlbertosTowardsPlayer(float Delta)
{
	if (bRotateAlbertosTowardPlayer == false)
		return;

	FRotator RotatorLook = AlbertosOwner->GetActorRotation();
	RotatorLook.Yaw = UKismetMathLibrary::FindLookAtRotation(AlbertosOwner->GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).Yaw;
	FRotator NewRotation = UKismetMathLibrary::RInterpTo(AlbertosOwner->GetActorRotation(), RotatorLook, Delta, 8.f);
	AlbertosOwner->SetActorRotation(NewRotation);

	if (FMath::IsNearlyEqual(RotatorLook.Yaw, NewRotation.Yaw, 2.f))
		bRotateAlbertosTowardPlayer = false;
}
#pragma endregion
