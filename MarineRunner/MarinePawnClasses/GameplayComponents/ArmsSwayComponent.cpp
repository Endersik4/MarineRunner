// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/MarinePawnClasses/GameplayComponents/ArmsSwayComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

// Sets default values for this component's properties
UArmsSwayComponent::UArmsSwayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UArmsSwayComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AMarineCharacter>(GetOwner());
	RelativeLocationInPawn = Player->GetArmsSkeletalMesh()->GetRelativeLocation();
}


// Called every frame
void UArmsSwayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GunSwayWhileMoving();
	GunSway(DeltaTime);

}

void UArmsSwayComponent::GunSway(float Delta)
{
	if (IsValid(Player) == false)
		return;

	FVector GunLocationInterp = Player->GetArmsSkeletalMesh()->GetRelativeLocation();

	bool bADSGunSway = CalculateGunSway(GunLocationInterp, GunRotationSway, Delta);
	if (bADSGunSway == true) return;

	Player->GetArmsSkeletalMesh()->SetRelativeLocation(GunLocationInterp);
	Player->GetArmsSkeletalMesh()->SetRelativeRotation(GunRotationSway);

	//If Player doing nothing then Gun goes back to its original position
	if (Player->GetInputAxisValue("Forward") == 0 && Player->GetInputAxisValue("Right") == 0)
	{
		FVector BackToOriginalPosition = FMath::VInterpTo(Player->GetArmsSkeletalMesh()->GetRelativeLocation(), RelativeLocationInPawn, Delta, 1.f);
		Player->GetArmsSkeletalMesh()->SetRelativeLocation(BackToOriginalPosition);
	}
}
bool UArmsSwayComponent::CalculateGunSway(FVector& CalculatedLocation, FRotator& CalculatedRotation, float Delta)
{
	//Preparing variables
	float LookUp = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookUp");
	float LookRight = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetInputAxisValue("LookRight");
	float Forward = Player->GetInputAxisValue("Forward");
	float Right = Player->GetInputAxisValue("Right");

	//Rotation Sway when The Player moves the mouse
	float InterpLookUp = FMath::FInterpTo(GunRotationSway.Pitch, UKismetMathLibrary::MapRangeClamped(LookUp, -1, 1, RotationSwayPitchRangeBack, RotationSwayPitchRangeUp), Delta, SpeedOfSwayPitch);
	float InterpLookRight = FMath::FInterpTo(GunRotationSway.Yaw, UKismetMathLibrary::MapRangeClamped(LookRight, -1, 1, RotationSwayYawRangeBack, RotationSwayYawRangeUp), Delta, SpeedOfSwayYaw);

	//Location Sway when The Player moves around
	float InterpForward = FMath::InterpEaseInOut(CalculatedLocation.X, UKismetMathLibrary::MapRangeClamped(Forward, 1, -1, RelativeLocationInPawn.X + LocationSwayXRangeBack,
		RelativeLocationInPawn.X + LocationSwayXRangeUp), (Delta * SpeedOfSwayX), 1.f);
	float InterpRight = FMath::InterpEaseInOut(CalculatedLocation.Y, UKismetMathLibrary::MapRangeClamped(Right, -1, 1, RelativeLocationInPawn.Y + LocationSwayYRangeBack,
		RelativeLocationInPawn.Y + LocationSwayYRangeUp), (Delta * SpeedOfSwayY), 1.f);

	CalculatedRotation = FRotator(InterpLookUp, InterpLookRight, 0.f);
	CalculatedLocation = FVector(InterpForward, InterpRight, CalculatedLocation.Z);

	return false;
}

void UArmsSwayComponent::GunSwayWhileMoving()
{
	if (IsValid(Player) == false) 
		return;
	if (!Player->GetIsWallrunning() && Player->GetInputAxisValue("Forward") == 0 && Player->GetInputAxisValue("Right") == 0) return;

	FVector CalculatedGunSway = CalculateLOBGunSwayWhileMoving();

	Player->GetArmsSkeletalMesh()->SetRelativeLocation(CalculatedGunSway);
}

FVector UArmsSwayComponent::CalculateLOBGunSwayWhileMoving()
{
	float SpeedOfLemniscate = GetWorld()->GetTimeSeconds() * SpeedOfSwayWhileMoving;

	float Ang = 2 / (9.f - FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate * 2)));
	float LocY = Ang * FMath::Cos(FMath::DegreesToRadians(SpeedOfLemniscate)); //SineWave
	float LocZ = (FMath::Sin(FMath::DegreesToRadians(SpeedOfLemniscate * 2)) * Ang) / 2; //Angle Offset

	FVector CalculatedGunSway = Player->GetArmsSkeletalMesh()->GetRelativeLocation();
	CalculatedGunSway.Y += (LocY * MultiplierOfLocationYSwayWhileMoving);
	CalculatedGunSway.Z += (LocZ * MultiplierOfLocationZSwayWhileMoving);

	return CalculatedGunSway;
}
