// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Albertos/Components/PlayerIsNearAlbertosComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MediaPlayer.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/AlbertosAIController.h"
#include "MarineRunner/Albertos/Components/CraftItemAlbertosComponent.h"

UPlayerIsNearAlbertosComponent::UPlayerIsNearAlbertosComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerIsNearAlbertosComponent::BeginPlay()
{
	Super::BeginPlay();

	SetUpAlbertosAndPlayerVariables();
	
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
	if (!IsValid(AlbertosAIController) || !IsValid(Player) || !IsValid(AlbertosOwner))
		return;

	if (bIgnorePlayer)
		return;

	const float& DistanceToPlayer = FVector::Distance(Player->GetActorLocation(), AlbertosOwner->GetActorLocation());
	if (DistanceToPlayer <= MaxPlayerDistanceToActiveAlbertos)
	{
		PlayerIsCloseToAlbertos();
	}
	else
	{
		GoBackToWendering();
	}
}

void UPlayerIsNearAlbertosComponent::PlayerIsCloseToAlbertos()
{
	if (bPlayerIsClose)
		return;

	AlbertosAIController->SetCanMove(false);
	AlbertosAIController->StopMovement();

	bRotateAlbertosTowardPlayer = true;

	bPlayerIsClose = true;
}

// if item is being crafted then wait till finish and then start wendering 
void UPlayerIsNearAlbertosComponent::GoBackToWendering()
{
	if (!bPlayerIsClose || AlbertosOwner->GetCraftItemAlbertosComponent()->IsCraftedItemValid())
		return;

	AlbertosAIController->SetCanMove(true);

	if (AlbertosOwner->GetIsCraftingTableWidgetVisible())
		AlbertosOwner->ToggleInventoryVisibility();

	OpenAlbertosDoor(false);

	bPlayerIsClose = false;
}

void UPlayerIsNearAlbertosComponent::OpenAlbertosDoor(const bool bOpenDoor)
{
	if (bIsFrontDoorOpen == bOpenDoor || !IsValid(AlbertosOwner))
		return;

	if (!IsValid(AlbertosOwner->GetAlbertosSkeletal()))
		return;
	if (!IsValid(AlbertosOwner->GetAlbertosSkeletal()->GetAnimInstance()))
		return;

	if (bOpenDoor)
	{
		if (IsValid(AlbertosOpenAnimationMontage))
			AlbertosOwner->GetAlbertosSkeletal()->GetAnimInstance()->Montage_Play(AlbertosOpenAnimationMontage);
		else
			UE_LOG(LogTemp, Warning, TEXT("Albertos Open Animation Montage is Nullptr in PlayerIsNearComponent!"));

		if (IsValid(AlbertosMediaPlayer))
			AlbertosMediaPlayer->Play();
		else
			UE_LOG(LogTemp, Warning, TEXT("Albertos Media Player is Nullptr in PlayerIsNearComponent!"));
	}
	else
	{
		if (IsValid(AlbertosCloseAnimationMontage))
			AlbertosOwner->GetAlbertosSkeletal()->GetAnimInstance()->Montage_Play(AlbertosCloseAnimationMontage);
		else
			UE_LOG(LogTemp, Warning, TEXT("Albertos Open Animation Montae is Nullptr in PlayerIsNearComponent!"));

		if (IsValid(AlbertosMediaPlayer))
			AlbertosMediaPlayer->Pause();
		else
			UE_LOG(LogTemp, Warning, TEXT("Albertos Media Player is Nullptr in PlayerIsNearComponent!"));
	}

	bIsFrontDoorOpen = bOpenDoor;

	if (IsValid(OpenDoorSound))
		UGameplayStatics::SpawnSoundAttached(OpenDoorSound, AlbertosOwner->GetAlbertosSkeletal(), OpenDoorSoundSocketName);
	else
		UE_LOG(LogTemp, Warning, TEXT("Open Door Sound is Nullptr in PlayerIsNearComponent!"));
}

void UPlayerIsNearAlbertosComponent::RotateAlbertosTowardsPlayer(float Delta)
{
	if (!IsValid(AlbertosOwner)|| !IsValid(Player))
		return; 

	if (!bRotateAlbertosTowardPlayer)
		return;

	FRotator RotationTowardsPlayer = AlbertosOwner->GetActorRotation();
	RotationTowardsPlayer.Yaw = UKismetMathLibrary::FindLookAtRotation(AlbertosOwner->GetActorLocation(), Player->GetActorLocation()).Yaw;

	const FRotator & RotatedAlbertosRotation = UKismetMathLibrary::RInterpTo(AlbertosOwner->GetActorRotation(), RotationTowardsPlayer, Delta, SpeedToRotateTowardsPlayer);
	AlbertosOwner->SetActorRotation(RotatedAlbertosRotation);

	if (FMath::IsNearlyEqual(RotationTowardsPlayer.Yaw, RotatedAlbertosRotation.Yaw, ToleranceToRotateAlbertos))
	{
		bRotateAlbertosTowardPlayer = false;
	}
}
#pragma endregion

void UPlayerIsNearAlbertosComponent::SetUpAlbertosAndPlayerVariables()
{
	TObjectPtr<AActor> AlbertosActor = GetOwner();
	if (ensureMsgf(IsValid(AlbertosActor), TEXT("Albertos Actor is nullptr in PlayerIsNearComponent")))
	{
		AlbertosOwner = Cast<AAlbertosPawn>(GetOwner());
	}

	TObjectPtr<AController> AlbertosController = AlbertosOwner->GetController();
	if (ensureMsgf(IsValid(AlbertosActor), TEXT("Albertos Controller is nullptr in PlayerIsNearComponent")))
	{
		AlbertosAIController = Cast<AAlbertosAIController>(AlbertosOwner->GetController());
	}

	TObjectPtr<APawn> PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (ensureMsgf(IsValid(AlbertosActor), TEXT("Player is nullptr in PlayerIsNearComponent")))
	{
		Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
}