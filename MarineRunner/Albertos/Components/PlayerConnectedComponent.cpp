// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Components/PlayerConnectedComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"
#include "MarineRunner/Albertos/Widgets/AlbertosHUDWidget.h"

UPlayerConnectedComponent::UPlayerConnectedComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerConnectedComponent::BeginPlay()
{
	Super::BeginPlay();

	SetUpPlayerAsAlbertos();

	FOnTimelineFloat SprintFOVTimelineFloat;
	SprintFOVTimelineFloat.BindUFunction(this, FName("SprintFOVTimelineProgress"));
	SprintFOVTimeline.AddInterpFloat(SprintStartCurve, SprintFOVTimelineFloat, NAME_None, TEXT("SprintFOVCurve"));
}

void UPlayerConnectedComponent::SetUpPlayerAsAlbertos()
{
	if (!IsValid(GetOwner()))
		return;

	AlbertosOwner = Cast<AAlbertosPawn>(GetOwner());

	if (!IsValid(AlbertosOwner))
		return;

	OrginalAlbertosMaxSpeed = AlbertosOwner->GetAlbertosFloatingMovement()->GetMaxSpeed();
	OrginalAcceleration = AlbertosOwner->GetAlbertosFloatingMovement()->Acceleration;
	CurrentSprintTime = SprintingTime;
}

void UPlayerConnectedComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RotateMeshTowardsCameraDir(DeltaTime);

	Sprint(DeltaTime);

	SprintFOVTimeline.TickTimeline(DeltaTime);
}

#pragma region ////////// movement ////////

void UPlayerConnectedComponent::MoveForward(float Axis)
{
	if (!bPlayersPossessed)
		return;

	const FVector DirectionToMove = AlbertosOwner->GetAlbertosCamera()->GetForwardVector() * Axis * FVector(1.f, 1.f, 0.f);
	AlbertosOwner->AddMovementInput(DirectionToMove, MovementInputScale);
}

void UPlayerConnectedComponent::MoveRight(float Axis)
{
	if (!bPlayersPossessed)
		return;

	const FVector DirectionToMove = AlbertosOwner->GetAlbertosCamera()->GetRightVector() * Axis * FVector(1.f, 1.f, 0.f);
	AlbertosOwner->AddMovementInput(DirectionToMove, MovementInputScale);
}

void UPlayerConnectedComponent::RotateMeshTowardsCameraDir(float Delta)
{
	if (!bPlayersPossessed)
		return;

	if (!IsValid(AlbertosOwner))
		return;

	if (AlbertosOwner->GetInputAxisValue(FName("Forward")) == 0 && AlbertosOwner->GetInputAxisValue(FName("Right")) == 0)
		return;

	const FRotator TargetRotation = FRotator(0.f, GetCurrentYawMoveDirection(), 0.f);
	const FRotator AlbertosRot = FRotator(0.f, AlbertosOwner->GetAlbertosSkeletal()->GetComponentRotation().Yaw, 0.f);
	const FRotator NewAlbertosSkeletalRotation = UKismetMathLibrary::RInterpTo(AlbertosRot, TargetRotation, Delta, MeshRotationSpeed);

	AlbertosOwner->GetAlbertosSkeletal()->SetWorldRotation(NewAlbertosSkeletalRotation);
}

float UPlayerConnectedComponent::GetCurrentYawMoveDirection()
{
	const float ForwardVal = AlbertosOwner->GetInputAxisValue(FName("Forward"));
	const float RightVal = AlbertosOwner->GetInputAxisValue(FName("Right"));
	const float CurrentCameraYaw = AlbertosOwner->GetController()->GetControlRotation().Yaw - 180.f;

	if (ForwardVal < 0 && RightVal < 0) // left bottom corner
		return (CurrentCameraYaw - 180.f) + 45.f;
	else if (ForwardVal < 0 && RightVal > 0) // right bottom corner
		return (CurrentCameraYaw - 180.f) - 45.f;
	else if (ForwardVal > 0 && RightVal > 0) // left top corner
		return CurrentCameraYaw + 45.f;
	else if (ForwardVal > 0 && RightVal < 0) // right top corner
		return CurrentCameraYaw - 45.f;
	else if (ForwardVal < 0) // bottom
		return CurrentCameraYaw - 180.f;
	else if (RightVal < 0) // left
		return CurrentCameraYaw - 90.f;
	else if (RightVal > 0) // right
		return CurrentCameraYaw + 90.f;
	else // top
		return CurrentCameraYaw;
}
#pragma endregion

#pragma region /////////// sprint //////////////////

void UPlayerConnectedComponent::SprintPressed()
{
	if (CurrentSprintTime < SprintingTime)
		return;

	bSprint = true;
	bIsSprinting = true;

	AlbertosOwner->GetAlbertosFloatingMovement()->MaxSpeed = SprintSpeed;
	AlbertosOwner->GetAlbertosFloatingMovement()->Acceleration = SprintAcceleration;

	SprintFOVTimeline.SetFloatCurve(SprintStartCurve, TEXT("SprintFOVCurve"));
	SprintFOVTimeline.PlayFromStart();
}

void UPlayerConnectedComponent::Sprint(float Delta)
{
	if (!bSprint)
		return;

	if (!IsValid(AlbertosHUDWidget))
		return;

	if (bIsSprinting)
	{
		CurrentSprintTime -= Delta;

		if (CurrentSprintTime <= 0.f)
			SprintReleased();
	}
	else if (CurrentSprintTime <= SprintingTime)
	{
		CurrentSprintTime += Delta * SprintRenewelTimeMultiplier;
	}
	else
	{
		AlbertosHUDWidget->ActivateSprintBar();
		bSprint = false;
	}

	AlbertosHUDWidget->UpdateSprintBarValue(CurrentSprintTime / SprintingTime);
}

void UPlayerConnectedComponent::SprintReleased()
{
	if (!bIsSprinting)
		return;

	bIsSprinting = false;

	AlbertosOwner->GetAlbertosFloatingMovement()->MaxSpeed = OrginalAlbertosMaxSpeed;
	AlbertosOwner->GetAlbertosFloatingMovement()->Acceleration = OrginalAcceleration;

	if (IsValid(AlbertosHUDWidget))
		AlbertosHUDWidget->ActivateSprintBar(false);

	SprintFOVTimeline.SetFloatCurve(SprintStopCurve, TEXT("SprintFOVCurve"));
	SprintFOVTimeline.PlayFromStart();
}

void UPlayerConnectedComponent::SprintFOVTimelineProgress(float NewFOV)
{
	if (!IsValid(AlbertosOwner))
		return;

	AlbertosOwner->GetAlbertosCamera()->SetFieldOfView(NewFOV);
}
#pragma endregion

void UPlayerConnectedComponent::PossessedBy(AController* NewController)
{
	if (!IsValid(NewController) || !IsValid(AlbertosOwner) || !IsValid(AlbertosHUDClass))
		return;

	if (!NewController->IsPlayerController())
		return;

	bPlayersPossessed = true;
	AlbertosOwner->bUseControllerRotationYaw = false;

	TObjectPtr<UUserWidget> SpawnedAlbertosHUDWidget = CreateWidget(UGameplayStatics::GetPlayerController(GetWorld(), 0), AlbertosHUDClass);
	if (!IsValid(SpawnedAlbertosHUDWidget))
		return;

	SpawnedAlbertosHUDWidget->AddToViewport();

	AlbertosHUDWidget = Cast<UAlbertosHUDWidget>(SpawnedAlbertosHUDWidget);
}