// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/AlbertosClasses/Components/PlayerIsNearAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MediaPlayer.h"

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
	
	if (IsValid(AlbertosMediaPlayer))
		AlbertosMediaPlayer->OpenSource(AlbertosMediaSource);

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
	if (IsValid(AlbertosAI) == false || IsValid(Player) == false || IsValid(AlbertosOwner) == false)
		return;

	if (bIgnorePlayer == true)
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

	if (AlbertosOwner->GetIsCraftingTableWidgetVisible() == true)
		AlbertosOwner->ToggleInventoryVisibility();

	OpenAlbertosDoor(false);

	bPlayerIsClose = false;
}

void UPlayerIsNearAlbertosComponent::OpenAlbertosDoor(bool bOpenDoor)
{
	if (bIsFrontDoorOpen == bOpenDoor || !IsValid(AlbertosOwner))
		return;

	if (bOpenDoor == true)
	{
		if (IsValid(AlbertosOpenAnimationMontage))
			AlbertosOwner->GetAlbertosSkeletal()->GetAnimInstance()->Montage_Play(AlbertosOpenAnimationMontage);

		if (IsValid(AlbertosMediaPlayer))
			AlbertosMediaPlayer->Play();
	}
	else
	{
		if (IsValid(AlbertosCloseAnimationMontage))
			AlbertosOwner->GetAlbertosSkeletal()->GetAnimInstance()->Montage_Play(AlbertosCloseAnimationMontage);

		if (IsValid(AlbertosMediaPlayer))
			AlbertosMediaPlayer->Pause();
	}

	bIsFrontDoorOpen = bOpenDoor;

	if (IsValid(OpenDoorSound))
		UGameplayStatics::SpawnSoundAttached(OpenDoorSound, AlbertosOwner->GetAlbertosSkeletal(), OpenDoorSoundSocketName);
}

void UPlayerIsNearAlbertosComponent::RotateAlbertosTowardsPlayer(float Delta)
{
	if (!IsValid(AlbertosOwner)|| !IsValid(Player))
		return; 

	if (bRotateAlbertosTowardPlayer == false)
		return;

	FRotator RotationTowardsPlayer = AlbertosOwner->GetActorRotation();
	RotationTowardsPlayer.Yaw = UKismetMathLibrary::FindLookAtRotation(AlbertosOwner->GetActorLocation(), Player->GetActorLocation()).Yaw;
	FRotator RotatedAlbertos = UKismetMathLibrary::RInterpTo(AlbertosOwner->GetActorRotation(), RotationTowardsPlayer, Delta, SpeedToRotateTowardsPlayer);
	AlbertosOwner->SetActorRotation(RotatedAlbertos);

	if (FMath::IsNearlyEqual(RotationTowardsPlayer.Yaw, RotatedAlbertos.Yaw, ToleranceToRotateAlbertos))
	{
		bRotateAlbertosTowardPlayer = false;
	}
}
#pragma endregion
