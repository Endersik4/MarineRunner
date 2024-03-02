// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/TypesOfEnemy/PunchingEnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/InteractInterface.h"

APunchingEnemyPawn::APunchingEnemyPawn()
{
}

void APunchingEnemyPawn::PlayerDetected(bool bDetected, APawn* DetectedPawn)
{
	DetectedPlayer = DetectedPawn;

	if (bDetected == true)
		GetWorld()->GetTimerManager().SetTimer(PlayerCloseForHitHandle, this, &APunchingEnemyPawn::IsPlayerCloseForHit, PlayerCloseRaycastInterval, true);
	else
		GetWorld()->GetTimerManager().ClearTimer(PlayerCloseForHitHandle);
}

void APunchingEnemyPawn::IsPlayerCloseForHit()
{
	if (IsValid(DetectedPlayer) == false || bIsPunching == true || bIsDead == true)
		return;

	FHitResult PlayerCloseHitResult;
	FVector HitPlayerRaycastStart = EnemySkeletalMesh->GetSocketLocation(PlayerCloseRaycastSocketNameLocation);
	FVector HitPlayerRaycastEnd = HitPlayerRaycastStart + UKismetMathLibrary::GetForwardVector(EnemySkeletalMesh->GetSocketRotation(PlayerCloseRaycastSocketNameLocation)) * MaxPlayerCloseRaycastDistance;
	bool bPlayerIsClose = GetWorld()->LineTraceSingleByChannel(PlayerCloseHitResult, HitPlayerRaycastStart, HitPlayerRaycastEnd, ECC_GameTraceChannel3);
	
	if (bPlayerIsClose == false)
		return;
	DamageSphereLocation = HitPlayerRaycastEnd;
	StartPunchingPlayer();
}

void APunchingEnemyPawn::StartPunchingPlayer()
{
	bIsPunching = true;
	PlayPunchAnimMontage();

	FTimerHandle ApplyDamageHandle;
	GetWorld()->GetTimerManager().SetTimer(ApplyDamageHandle, this, &APunchingEnemyPawn::ApplyDamageToPlayer, ApplyDamageTime, false);

	FTimerHandle DamageAnimFinishedHandle;
	GetWorld()->GetTimerManager().SetTimer(DamageAnimFinishedHandle, this, &APunchingEnemyPawn::DamageAnimFinished, CanDamageAgainTime, false);
}

void APunchingEnemyPawn::ApplyDamageToPlayer()
{
	if (PunchSound)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PunchSound, EnemySkeletalMesh->GetSocketLocation(PlayerCloseRaycastSocketNameLocation));

	FHitResult DamagePlayerResult;
	
	bool bPlayerTakenDamage = GetWorld()->SweepSingleByChannel(DamagePlayerResult, DamageSphereLocation, DamageSphereLocation, FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(DamageSphereRadius));
	if (bPlayerTakenDamage == false)
		return;

	IInteractInterface* ActorWithDamageInterface = Cast<IInteractInterface>(DamagePlayerResult.GetActor());
	if (ActorWithDamageInterface == nullptr)
		return;

	if (ActorWithDamageInterface == this)
		return;

	ActorWithDamageInterface->ApplyDamage(Damage, DamageImpulseForce, DamagePlayerResult, this);
}

void APunchingEnemyPawn::DamageAnimFinished()
{
	bIsPunching = false;
}
