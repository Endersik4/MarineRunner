// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Gun/Mods/ScopeGunMod.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"

#include "MarineRunner/Gun/Gun.h"

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

	if (!Gun->GetGunSkeletalMesh()->GetMaterial(ZoomMaterialIndexOnWeapon))
		return;

	OwningGun = Gun;
}

int32 AScope::Zoom(float WheelAxis, bool bShouldRestartScope)
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
		return CurrentScope;

	if (IsValid(ZoomSound)) 
		UGameplayStatics::PlaySound2D(GetWorld(), ZoomSound);

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
			OwningGun->GetGunSkeletalMesh()->SetMaterial(ZoomMaterialIndexOnWeapon, ZoomRenderTargetMaterial);
	}
	else
	{ 
		if (ZoomNotActiveMaterial)
			OwningGun->GetGunSkeletalMesh()->SetMaterial(ZoomMaterialIndexOnWeapon, ZoomNotActiveMaterial);
	}

	ZoomCamera->SetActive(bShouldActive);
}

