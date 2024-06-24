// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Components/QuickAttackComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/Player/MarinePlayer.h"
#include "MarineRunner/Interfaces/DamageInterface.h"
#include "MarineRunner/Weapon/WeaponBase.h"
#include "WeaponHandlerComponent.h"

UQuickAttackComponent::UQuickAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UQuickAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(IsValid(GetOwner()), TEXT("Player is nullptr in Quick Attack Component!")))
	{
		Player = Cast<AMarineCharacter>(GetOwner());
	}	
	if (ensureMsgf(IsValid(UGameplayStatics::GetPlayerController(GetWorld(), 0)), TEXT("Player Controller is nullptr in Quick Attack Component!")))
	{
		PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	}
}

#pragma region /////////////////// QUICK ATTACK ////////////
void UQuickAttackComponent::QuickAttack()
{
	if (!CanPlayerQuickAttack())
		return;

	bCanQuickAttack = false;
	GetWorld()->GetTimerManager().SetTimer(CanQuickAttackAgainHandle, this, &UQuickAttackComponent::SetCanQuickAttackAgain, CanAttackAgainTime, false);

	GetWorld()->GetTimerManager().SetTimer(QuickAttackHandle, this, &UQuickAttackComponent::QuickAttackFinished, QuickAttackTime, false);

	SpawnQuickAttackWeapon();

	QuickAttackEffects();

	QuickAttackHit();
}

void UQuickAttackComponent::SpawnQuickAttackWeapon()
{
	if (!WeaponToSpawn.TryLoadClass<UObject>())
		return;

	SpawnedQuickWeapon = GetWorld()->SpawnActor<AActor>(WeaponToSpawn.TryLoadClass<UObject>(), FVector(0.f), FRotator(0.f));
	if (!IsValid(SpawnedQuickWeapon))
		return;

	if (IsValid(Player->GetWeaponHandlerComponent()->GetCurrentWeapon()))
	{
		Player->GetWeaponHandlerComponent()->SetWeaponHiddenByPlayer(true);
		Player->GetWeaponHandlerComponent()->GetCurrentWeapon()->PutAwayWeapon(true, true);
	}

	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(false);

	SpawnedQuickWeapon->AttachToComponent(Player->GetArmsSkeletalMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketNameToAttach);
}

void UQuickAttackComponent::QuickAttackEffects()
{
	Player->GetArmsSkeletalMesh()->SetForceRefPose(false);

	if (IsValid(QuickAttackAnim))
		Player->GetArmsSkeletalMesh()->PlayAnimation(QuickAttackAnim, false);

	if (IsValid(QuickAttackSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), QuickAttackSound);

	if (IsValid(PlayerController))
		PlayerController->ClientStartCameraShake(QuickAttackCameraShake);
}

void UQuickAttackComponent::QuickAttackHit()
{
	FHitResult QuickAttackResult;
	const FVector StartRaycast = Player->GetCameraLocation();
	const FVector EndRaycast = StartRaycast + MaxDistanceToDamage * Player->GetCamera()->GetForwardVector();
	TArray<AActor*> ActorsToIgnore = { Player };

	const bool bHit = UKismetSystemLibrary::BoxTraceSingle(GetWorld(), StartRaycast, EndRaycast, SweepBoxSize, FRotator(0.f),
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel12), false, ActorsToIgnore, EDrawDebugTrace::None, QuickAttackResult, true);
	if (!bHit)
		return;

	IDamageInterface* ObjectThatHasInterface = Cast<IDamageInterface>(QuickAttackResult.GetActor());
	if (ObjectThatHasInterface)
	{
		ObjectThatHasInterface->ApplyDamage(Damage, DamageInterfaceImpulseForce, QuickAttackResult, SpawnedQuickWeapon, WeaponType);
	}
	else
	{
		HitWithoutDamageInterface(QuickAttackResult);
	}
}

void UQuickAttackComponent::HitWithoutDamageInterface(const FHitResult& HitResultFromAttack)
{
	if (HitResultFromAttack.GetComponent()->IsSimulatingPhysics())
	{
		const FVector Impulse = Player->GetCamera()->GetForwardVector() * DefaultHitImpulseForce;
		HitResultFromAttack.GetComponent()->AddImpulse(Impulse);
	}

	if (IsValid(DefaultHitSound))
		UGameplayStatics::SpawnSound2D(GetWorld(), DefaultHitSound);

	SpawnHitDecal(HitResultFromAttack);
}

void UQuickAttackComponent::SpawnHitDecal(const FHitResult& HitResultFromAttack)
{
	if (!IsValid(WeaponHoleDecalMaterial))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon hole Decal is nullptr in Quick Attack component!"));
		return;
	}

	const FRotator DecalRotation = HitResultFromAttack.ImpactNormal.Rotation() + FRotator(0.f, Player->GetActorRotation().Yaw, 0.f);
	TObjectPtr<UDecalComponent>	SpawnedDecal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), WeaponHoleDecalMaterial, FVector(1.f), HitResultFromAttack.ImpactPoint, DecalRotation);
	if (!IsValid(SpawnedDecal))
		return;

	SpawnedDecal->DecalSize = WeaponHoleDecalSize;
	SpawnedDecal->SetFadeScreenSize(0.f);
	SpawnedDecal->SetFadeOut(HitDecalFadeOutDelay, HitDecalFadeOutDuration, false);
}

void UQuickAttackComponent::QuickAttackFinished()
{
	if (IsValid(SpawnedQuickWeapon))
		SpawnedQuickWeapon->Destroy();

	Player->GetArmsSkeletalMesh()->SetForceRefPose(true);

	if (IsValid(Player->GetWeaponHandlerComponent()->GetCurrentWeapon()))
	{
		Player->GetWeaponHandlerComponent()->SetWeaponHiddenByPlayer(false);
		Player->GetWeaponHandlerComponent()->GetCurrentWeapon()->DrawWeapon();
	}

	Player->GetWeaponHandlerComponent()->SetCanChangeWeapon(true);
}

bool UQuickAttackComponent::CanPlayerQuickAttack()
{
	if (!bCanQuickAttack)
		return false;

	if (!IsValid(QuickAttackAnim))
		return false;

	if (!IsValid(Player))
		return false;

	if (Player->GetIsInCutscene())
		return false;

	return true;
}

#pragma endregion