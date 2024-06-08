// Copyright Adam Bartela.All Rights Reserved


#include "GunRecoilComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Weapon/Gun/Gun.h"
#include "GunReloadComponent.h"
#include "MarineRunner/Player/MarinePlayerController.h"

UGunRecoilComponent::UGunRecoilComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CameraRecoilTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("Shoot Recoil Timeline"));
}

void UGunRecoilComponent::BeginPlay()
{
	Super::BeginPlay();

	SetTimelineAccordingToRecoilType();

	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player Controller is nullptr in GunRecoilComponenet!")))
	{
		PlayerController = Cast<AMarinePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Gun is nullptr in GunRecoilComponenet!")))
	{
		Gun = Cast<AGun>(GetOwner());
	}
}

void UGunRecoilComponent::SetTimelineAccordingToRecoilType()
{
	FOnTimelineFloat TimelineFloatProgress;
	if (RecoilType == ERecoilType::ERT_RandomValue)
	{
		TimelineFloatProgress.BindUFunction(this, "RandomCameraRecoilTimelineProgress");
		CameraRecoilTimeline->AddInterpFloat(RandomCameraRecoil_SmoothCurve, TimelineFloatProgress);
	}
	else if (RecoilType == ERecoilType::ERT_CurveRecoil)
	{
		TimelineFloatProgress.BindUFunction(this, "CurveCameraRecoilTimelineProgress");
		CameraRecoilTimeline->AddInterpFloat(YawCameraRecoilCurve, TimelineFloatProgress);
	}
}

void UGunRecoilComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RotatePlayerCameraToPitchDistance(DeltaTime);
	CameraInterpBackToInitialPosition(DeltaTime);
}

void UGunRecoilComponent::ShootPressed()
{
	PlayRecoil();
}

void UGunRecoilComponent::ShootReleased()
{
	if (RecoilType != ERecoilType::ERT_CurveRecoil)
		return;

	if (bCanStartCameraRecoil) // make sure to do this function only one time
		return;

	GetWorld()->GetTimerManager().SetTimer(FirstBulletHandle, this, &UGunRecoilComponent::ShouldFirstBulletGoStraight, FirstBulletWithoutRecoilTime, false);
	ResetVariablesForCameraRecoil();
	BackPlayerCameraToInitialRotation();
}
#pragma region ////////////////////////////// CAMERA RECOIL ///////////////////////////////////
void UGunRecoilComponent::PlayRecoil()
{
	if (!IsValid(Gun) || !IsValid(PlayerController))
		return;

	if (RecoilCameraShake)
		PlayerController->ClientStartCameraShake(RecoilCameraShake);

	if (!bCanStartCameraRecoil)
		return;

	TargetPlayerCameraRotation = PlayerController->GetControlRotation();
	
	if (RecoilType == ERecoilType::ERT_CurveRecoil)
	{
		RandomYawOffset_CurveRecoil = FMath::FRandRange(RandomOffsetRecoilCurve.GetLowerBoundValue(), RandomOffsetRecoilCurve.GetUpperBoundValue());
		bRotateCameraToPitchDistance = true;

		bCanStartCameraRecoil = false;
	}
	else if (RecoilType == ERecoilType::ERT_RandomValue)
	{
		RandomCameraRecoilYaw = FMath::FRandRange(RandomCameraRecoilYawRange.GetLowerBoundValue(), RandomCameraRecoilYawRange.GetUpperBoundValue());
		RandomCameraRecoilPitch = FMath::FRandRange(RandomCameraRecoilPitchRange.GetLowerBoundValue(), RandomCameraRecoilPitchRange.GetUpperBoundValue());
	
		if (Gun->GetStatusOfGun() == EStatusOfAimedGun::ESAG_ADS)
		{
			RandomCameraRecoilYaw /= DividerOfRecoilWhileADS;
			RandomCameraRecoilPitch /= DividerOfRecoilWhileADS;
		}
	}

	CameraRecoilTimeline->PlayFromStart();
}

void UGunRecoilComponent::RandomCameraRecoilTimelineProgress(float SmoothValue)
{
	if (!IsValid(PlayerController))
		return;

	const float ControlRotationPitch = RandomCameraRecoilPitch * SmoothValue * UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	const float ControlRotationYaw = RandomCameraRecoilYaw * SmoothValue * UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	PlayerController->AddPitchInput(-ControlRotationPitch);
	PlayerController->AddYawInput(ControlRotationYaw);
}

void UGunRecoilComponent::CurveCameraRecoilTimelineProgress(float NewCameraYawRotation)
{
	if (!IsValid(Gun) || !IsValid(PlayerController))
		return;

	NewCameraYawRotation += RandomYawOffset_CurveRecoil;

	if (Gun->GetStatusOfGun() == EStatusOfAimedGun::ESAG_ADS)
		NewCameraYawRotation /= DividerOfRecoilWhileADS;

	NewCameraYawRotation *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	PlayerController->AddYawInput(NewCameraYawRotation);
}

void UGunRecoilComponent::RotatePlayerCameraToPitchDistance(float Delta)
{
	if (!bRotateCameraToPitchDistance || !IsValid(Gun) || !IsValid(PlayerController))
		return;

	if (!IsValid(Gun->GetGunReloadComponent()))
		return;

	// Calculates how much should add to Pitch Input per one bullet according to full pitch distance with full magazine
	float ControlRotationPitch = (PitchDistanceFromStart * Delta) / (Gun->GetGunReloadComponent()->GetOriginalMagazineCapacity() * Gun->GetShootTime());
	if (Gun->GetStatusOfGun() == EStatusOfAimedGun::ESAG_ADS)
		ControlRotationPitch /= DividerOfRecoilWhileADS;

	ControlRotationPitch *= UGameplayStatics::GetGlobalTimeDilation(GetWorld());

	PlayerController->AddPitchInput(-ControlRotationPitch);
}

void UGunRecoilComponent::BackPlayerCameraToInitialRotation()
{
	if (!IsValid(PlayerController))
		return;

	const float DistanceBetweenPitch = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(PlayerController->GetControlRotation(), TargetPlayerCameraRotation).Pitch);
	
	//If distance is too big then camera doesnt go back to its inital rotation
	const float DistanceBetweenYaw = FMath::Abs(UKismetMathLibrary::NormalizedDeltaRotator(PlayerController->GetControlRotation(), TargetPlayerCameraRotation).Yaw);
	if (DistanceBetweenYaw > MaxDistanceForCameraToGoBack)
	{
		TargetPlayerCameraRotation.Yaw = PlayerController->GetControlRotation().Yaw;
	}
	if (DistanceBetweenPitch > PitchDistanceFromStart)
	{
		TargetPlayerCameraRotation = PlayerController->GetControlRotation();

		if (DistanceBetweenYaw < MaxYawDistanceToSubstract)
			TargetPlayerCameraRotation.Pitch -= PitchDistanceFromStart;
	}

	bRotatePlayerCameraToTargetRotation = true;
}

void UGunRecoilComponent::CameraInterpBackToInitialPosition(float Delta)
{
	if (!bRotatePlayerCameraToTargetRotation)
		return;

	if (!IsValid(PlayerController))
		return;

	// If the player moves the mouse, it stops the camera movement that was going back to its starting position.
	if (PlayerController->MouseXValue > MouseCancelCameraTolerance || PlayerController->MouseXValue < -MouseCancelCameraTolerance
		|| PlayerController->MouseYValue > MouseCancelCameraTolerance || PlayerController->MouseYValue < -MouseCancelCameraTolerance)
	{
		bRotatePlayerCameraToTargetRotation = false;
		return;
	}

	if (PlayerController->GetControlRotation().Equals(TargetPlayerCameraRotation, MouseCancelCameraTolerance))
	{
		bRotatePlayerCameraToTargetRotation = false;
		return;
	}

	const FRotator NewControlRotation = UKismetMathLibrary::RInterpTo(PlayerController->GetControlRotation(), TargetPlayerCameraRotation, Delta * UGameplayStatics::GetGlobalTimeDilation(GetWorld()), BackToInitialCameraRotationSpeed);
	PlayerController->SetControlRotation(NewControlRotation);
}

void UGunRecoilComponent::ResetVariablesForCameraRecoil()
{
	if (!IsValid(Gun))
		return;

	bCanStartCameraRecoil = true;

	Gun->SetConstantlyShoot(false);

	bRotateCameraToPitchDistance = false;

	if (RecoilType == ERecoilType::ERT_CurveRecoil)
		CameraRecoilTimeline->Stop();
}
#pragma endregion

const FRotator UGunRecoilComponent::RandomBulletRotation()
{
	if (bFirstBulletWithoutRecoil || !IsValid(Gun))
		return FRotator(0.f);

	float NewPitchBulletRotaton = FMath::FRandRange(RandomBulletRecoilPitch.GetLowerBoundValue(), RandomBulletRecoilPitch.GetUpperBoundValue());
	float NewYawBulletRotation = FMath::FRandRange(RandomBulletRecoilYaw.GetLowerBoundValue(), RandomBulletRecoilYaw.GetUpperBoundValue());
	if (Gun->GetStatusOfGun() == EStatusOfAimedGun::ESAG_ADS)
	{
		NewPitchBulletRotaton /= DividerOfBulletRecoilWhileADS;
		NewYawBulletRotation /= DividerOfBulletRecoilWhileADS;
	}

	return FRotator(NewPitchBulletRotaton, NewYawBulletRotation, 0.f);
}