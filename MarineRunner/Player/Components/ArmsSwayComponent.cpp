// Copyright Adam Bartela.All Rights Reserved


#include "ArmsSwayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Player/Components/WeaponHandlerComponent.h"

// Sets default values for this component's properties
UArmsSwayComponent::UArmsSwayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UArmsSwayComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Arms Sway Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}
	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player Controller is nullptr in Arms Sway Component!")))
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}

	if (IsValid(Player))
	{
		InitialArmsRelativeLocation = Player->GetArmsSkeletalMesh()->GetRelativeLocation();
	}
}


// Called every frame
void UArmsSwayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ArmsSway_MouseMove(DeltaTime);
	ArmsSway_WhileMoving();
}

void UArmsSwayComponent::ArmsSway_MouseMove(float Delta)
{
	if (!IsValid(Player) || !IsValid(PlayerController))
		return;

	FVector CalculatedArmsSwayLocation = Player->GetArmsSkeletalMesh()->GetRelativeLocation();
	CalculateArmsSway(CalculatedArmsSwayLocation, ArmsRotationSway, Delta);

	Player->GetArmsSkeletalMesh()->SetRelativeLocation(CalculatedArmsSwayLocation);
	Player->GetArmsSkeletalMesh()->SetRelativeRotation(ArmsRotationSway);

	//If Player doing nothing then arms goes back to its original position
	if ((Player->GetInputAxisValue("Forward") == 0 && Player->GetInputAxisValue("Right") == 0))
	{
		const FVector& BackToOriginalPositionLocation = FMath::VInterpTo(Player->GetArmsSkeletalMesh()->GetRelativeLocation(), InitialArmsRelativeLocation, Delta, BackToInitialPositionSpeed);
		Player->GetArmsSkeletalMesh()->SetRelativeLocation(BackToOriginalPositionLocation);
	}
}
void UArmsSwayComponent::CalculateArmsSway(FVector& CalculatedLocation, FRotator& CalculatedRotation, const float& Delta)
{
	//Preparing variables
	const float& LookUp = PlayerController->GetInputAxisValue("LookUp");
	const float& LookRight = PlayerController->GetInputAxisValue("LookRight");
	const float& Forward = Player->GetInputAxisValue("Forward");
	const float& Right = Player->GetInputAxisValue("Right");

	//Rotation Sway when The Player moves the mouse
	float RotationSwayUP = FMath::FInterpTo(ArmsRotationSway.Pitch, UKismetMathLibrary::MapRangeClamped(LookUp, -1, 1, RotationSwayPitchRangeBack, RotationSwayPitchRangeUp), Delta, SpeedOfSwayPitch);
	float RotationSwayRight = FMath::FInterpTo(ArmsRotationSway.Yaw, UKismetMathLibrary::MapRangeClamped(LookRight, -1, 1, RotationSwayYawRangeBack, RotationSwayYawRangeUp), Delta, SpeedOfSwayYaw);

	float LocationSwayForward, LocationSwayRight;
	if (!Player->GetWeaponHandlerComponent()->GetIsPlayerInAds())
	{
		//Location Sway when The Player moves around
		LocationSwayForward = FMath::InterpEaseInOut(CalculatedLocation.X, UKismetMathLibrary::MapRangeClamped(Forward, 1, -1, InitialArmsRelativeLocation.X + LocationSwayXRangeBack,
			InitialArmsRelativeLocation.X + LocationSwayXRangeUp), (Delta * SpeedOfSwayX), 1.f);
		LocationSwayRight = FMath::InterpEaseInOut(CalculatedLocation.Y, UKismetMathLibrary::MapRangeClamped(Right, -1, 1, InitialArmsRelativeLocation.Y + LocationSwayYRangeBack,
			InitialArmsRelativeLocation.Y + LocationSwayYRangeUp), (Delta * SpeedOfSwayY), 1.f);
	}
	else
	{
		RotationSwayUP /= ArmsSwayDivider;
		RotationSwayRight /= ArmsSwayDivider;
		LocationSwayForward = FMath::InterpEaseInOut(CalculatedLocation.X, UKismetMathLibrary::MapRangeClamped(Forward, 1, -1, InitialArmsRelativeLocation.X + (LocationSwayXRangeBack / ArmsSwayDivider),
			InitialArmsRelativeLocation.X + (LocationSwayXRangeUp / ArmsSwayDivider)), (Delta * SpeedOfSwayX), 1.f);
		LocationSwayRight = FMath::InterpEaseInOut(CalculatedLocation.Y, UKismetMathLibrary::MapRangeClamped(Right, -1, 1, InitialArmsRelativeLocation.Y + (LocationSwayYRangeBack / ArmsSwayDivider),
			InitialArmsRelativeLocation.Y + (LocationSwayYRangeUp / ArmsSwayDivider)), (Delta * SpeedOfSwayY), 1.f);
	}

	CalculatedRotation = FRotator(RotationSwayUP, RotationSwayRight, 0.f);
	CalculatedLocation = FVector(LocationSwayForward, LocationSwayRight, CalculatedLocation.Z);
}

void UArmsSwayComponent::ArmsSway_WhileMoving()
{
	if (!IsValid(Player)) 
		return;
	if (!Player->GetIsWallrunning() && Player->GetInputAxisValue("Forward") == 0 && Player->GetInputAxisValue("Right") == 0)
		return;
	if (Player->GetVelocity().Length() < MinVelocityToStartArmsSway)
		return;

	const FVector& CalculatedGunSway = CalculateArmsSwayWhileMoving();
	Player->GetArmsSkeletalMesh()->SetRelativeLocation(CalculatedGunSway);
}

FVector UArmsSwayComponent::CalculateArmsSwayWhileMoving()
{
	const float& SpeedOfLemniscate = GetWorld()->GetTimeSeconds() * SpeedOfSwayWhileMoving;

	// Lemniscate of Bernoulli equation
	float Ang = 2 / (9.f - FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate * 2)));
	float LocY = Ang * FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate)); //SineWave
	float LocZ = (FMath::Sin(FMath::DegreesToRadians(SpeedOfLemniscate * 2)) * Ang) / 2; //Angle Offset

	if (Player->GetWeaponHandlerComponent()->GetIsPlayerInAds())
	{
		Ang /= ArmsSwayWhileMovingDivider;
		LocY /= ArmsSwayWhileMovingDivider;
		LocZ /= ArmsSwayWhileMovingDivider;
	}

	FVector CalculatedArmsSway = Player->GetArmsSkeletalMesh()->GetRelativeLocation();
	CalculatedArmsSway.Y += (LocY * MultiplierOfLocationYSwayWhileMoving);
	CalculatedArmsSway.Z += (LocZ * MultiplierOfLocationZSwayWhileMoving);

	return CalculatedArmsSway;
}

void UArmsSwayComponent::ResetArmsLocation()
{
	Player->GetArmsSkeletalMesh()->SetRelativeLocation(InitialArmsRelativeLocation);
}
