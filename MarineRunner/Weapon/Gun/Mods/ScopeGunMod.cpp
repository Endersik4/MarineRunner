// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Weapon/Gun/Mods/ScopeGunMod.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"

#include "MarineRunner/Weapon/Gun/Gun.h"

// Sets default values
AScope::AScope()
{
	PrimaryActorTick.bCanEverTick = false;

	//Configuring the SceneCaptureComponent responsible for capturing another screen for Scope
	ZoomCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ZoomCamera"));
	RootComponent = ZoomCamera;
	ZoomCamera->FOVAngle = 16.f;
	ZoomCamera->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;

	Tags.Add(FName("Scope"));
}

// Called when the game starts or when spawned
void AScope::BeginPlay()
{
	Super::BeginPlay();

	ZoomCamera->SetActive(false);
}

void AScope::SetUpZoomMaterial(TObjectPtr<AGun> Gun)
{
	if (!IsValid(Gun))
		return;

	if (!Gun->GetWeaponSkeletalMesh()->GetMaterial(ZoomMaterialIndexOnWeapon))
		return;

	OwningGun = Gun;
}

const int32 AScope::Zoom(const float WheelAxis, const bool bShouldRestartScope)
{
	if (Scope_FOVValues.Num() < 1) 
		return 0;

	if (bShouldRestartScope)
	{
		ChangeScope(0);
		return 0;
	}

	if (WheelAxis > 0.1f && CurrentScope + 1 < Scope_FOVValues.Num())
	{
		ChangeScope(CurrentScope + 1);
	}
	else if (WheelAxis < -0.1f && CurrentScope - 1 >= 0)
	{
		ChangeScope(CurrentScope - 1);
	}
	else
	{
		return CurrentScope;
	}

	if (IsValid(ZoomSound))
		UGameplayStatics::PlaySound2D(GetWorld(), ZoomSound);
	else
		UE_LOG(LogTemp, Warning, TEXT("Zoom Sound is nullptr in ScopeGunMode!"));

	return CurrentScope;
}

void AScope::ChangeScope(int32 NextScopeIndex)
{
	if (Scope_FOVValues.Num() < 1) 
		return;

	CurrentScope = NextScopeIndex;
	ZoomCamera->FOVAngle = Scope_FOVValues[NextScopeIndex];
}

void AScope::ActiveZoom(bool bShouldActive)
{
	if (bShouldActive)
	{
		if (ZoomRenderTargetMaterial)
			OwningGun->GetWeaponSkeletalMesh()->SetMaterial(ZoomMaterialIndexOnWeapon, ZoomRenderTargetMaterial);
		else
			UE_LOG(LogTemp, Warning, TEXT("Zoom Render Target Material is nullptr in ScopeGunMode!"));
	}
	else
	{ 
		if (ZoomNotActiveMaterial)
			OwningGun->GetWeaponSkeletalMesh()->SetMaterial(ZoomMaterialIndexOnWeapon, ZoomNotActiveMaterial);
		else
			UE_LOG(LogTemp, Warning, TEXT("Zoom Not Active Material is nullptr in ScopeGunMode!"));
	}

	ZoomCamera->SetActive(bShouldActive);
}

