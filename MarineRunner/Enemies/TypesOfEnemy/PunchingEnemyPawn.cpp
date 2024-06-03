// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Enemies/TypesOfEnemy/PunchingEnemyPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Interfaces/DamageInterface.h"

APunchingEnemyPawn::APunchingEnemyPawn()
{
}

void APunchingEnemyPawn::PlayerDetected(const bool bDetected, APawn* DetectedPawn)
{
	DetectedPlayer = DetectedPawn;
	
	if (bDetected)
		GetWorld()->GetTimerManager().SetTimer(PlayerCloseForHitHandle, this, &APunchingEnemyPawn::IsPlayerCloseForHit, PlayerCloseRaycastInterval, true);
	else
		GetWorld()->GetTimerManager().ClearTimer(PlayerCloseForHitHandle);
}

void APunchingEnemyPawn::IsPlayerCloseForHit()
{
	if (!IsValid(DetectedPlayer) || bIsPunching || bIsDead)
		return;

	FHitResult PlayerCloseHitResult;
	const FVector& HitPlayerRaycastStart = EnemySkeletalMesh->GetSocketLocation(PlayerCloseRaycastSocketNameLocation);
	const FVector& HitPlayerRaycastEnd = HitPlayerRaycastStart + UKismetMathLibrary::GetForwardVector(EnemySkeletalMesh->GetSocketRotation(PlayerCloseRaycastSocketNameLocation)) * MaxPlayerCloseRaycastDistance;
	bool bPlayerIsClose = GetWorld()->LineTraceSingleByChannel(PlayerCloseHitResult, HitPlayerRaycastStart, HitPlayerRaycastEnd, ECC_Pawn);
	
	if (!bPlayerIsClose)
		return;

	IDamageInterface* ActorWithDamageInterface = Cast<IDamageInterface>(PlayerCloseHitResult.GetActor());
	if (!ActorWithDamageInterface)
		return;

	DamageSphereLocation = HitPlayerRaycastEnd;
	StartPunchingPlayer();
}

void APunchingEnemyPawn::StartPunchingPlayer()
{
	bIsPunching = true;
	if (IsValid(PunchAnimMontage))
		EnemySkeletalMesh->GetAnimInstance()->Montage_Play(PunchAnimMontage);
	else
		UE_LOG(LogTemp, Warning, TEXT("Punch Anim Montage is nullptr in PunchingEnemyPawn!"));

	FTimerHandle ApplyDamageHandle;
	GetWorld()->GetTimerManager().SetTimer(ApplyDamageHandle, this, &APunchingEnemyPawn::ApplyDamageToPlayer, ApplyDamageTime, false);

	FTimerHandle DamageAnimFinishedHandle;
	GetWorld()->GetTimerManager().SetTimer(DamageAnimFinishedHandle, this, &APunchingEnemyPawn::DamageAnimFinished, CanDamageAgainTime, false);
}

void APunchingEnemyPawn::ApplyDamageToPlayer()
{
	if (PunchSound)
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), PunchSound, EnemySkeletalMesh->GetSocketLocation(PlayerCloseRaycastSocketNameLocation));
	else
		UE_LOG(LogTemp, Warning, TEXT("Punch Sound is nullptr in PunchingEnemyPawn!"));

	FHitResult DamagePlayerResult;
	
	bool bPlayerTakenDamage = GetWorld()->SweepSingleByChannel(DamagePlayerResult, DamageSphereLocation, DamageSphereLocation, FQuat::Identity, ECC_GameTraceChannel3, FCollisionShape::MakeSphere(DamageSphereRadius));
	if (!bPlayerTakenDamage)
		return;

	IDamageInterface* ActorWithDamageInterface = Cast<IDamageInterface>(DamagePlayerResult.GetActor());
	if (!ActorWithDamageInterface)
		return;

	if (ActorWithDamageInterface == this)
		return;

	ActorWithDamageInterface->ApplyDamage(Damage, DamageImpulseForce, DamagePlayerResult, this);
}

void APunchingEnemyPawn::DamageAnimFinished()
{
	bIsPunching = false;
}
