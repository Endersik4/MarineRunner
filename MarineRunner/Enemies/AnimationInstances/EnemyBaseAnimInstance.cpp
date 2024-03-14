// Copyright Adam Bartela.All Rights Reserved


#include "EnemyBaseAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

void UEnemyBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	PawnOwner = TryGetPawnOwner();
	if (IsValid(PawnOwner))
	{
		PreviousLocation = PawnOwner->GetActorLocation();
	}
}

void UEnemyBaseAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (DeltaTime == 0.f || !IsValid(PawnOwner))
		return;
	CurrentVelocity = ((PawnOwner->GetActorLocation() - PreviousLocation) / DeltaTime).Length();
	PreviousLocation = PawnOwner->GetActorLocation();
	
	CurrentSpeed = CurrentVelocity / SpeedDivider;
	CurrentRotatedActorAngle = UKismetMathLibrary::MakeRotFromX(UKismetMathLibrary::InverseTransformDirection(PawnOwner->GetActorTransform(), FVector(0.f))).Yaw;

}
