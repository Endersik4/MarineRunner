// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/GunClasses/Scope.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TextureRenderTarget2D.h"

#include "Gun.h"

// Sets default values
AScope::AScope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick =false;

	//Configuring the SceneCaptureComponent responsible for capturing another screen for Scope
	ZoomCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ZoomCamera"));
	RootComponent = ZoomCamera;
	ZoomCamera->FOVAngle = 16.f;
	ZoomCamera->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;
	//

	Tags.Add(FName("Scope"));
}

// Called when the game starts or when spawned
void AScope::BeginPlay()
{
	Super::BeginPlay();

}

void AScope::SetUpZoomMaterial(AGun* Gun)
{
	if (IsValid(Gun) == false)
		return;

	if (Gun->GetGunSkeletalMesh()->GetMaterial(ZoomMaterialIndexOnWeapon) == nullptr)
		return;

	DynamicScopeMaterial = UMaterialInstanceDynamic::Create(Gun->GetGunSkeletalMesh()->GetMaterial(ZoomMaterialIndexOnWeapon), Gun->GetGunSkeletalMesh());
	Gun->GetGunSkeletalMesh()->SetMaterial(ZoomMaterialIndexOnWeapon, DynamicScopeMaterial);
}

int32 AScope::Zoom(float WheelAxis, bool bShouldRestartScope)
{
	if (Scope_FOVValues.Num() < 1) return 0;

	if (bShouldRestartScope == true)
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
	else return CurrentScope;

	if (ZoomSound) 
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
		ChangeScopeResolution(ZoomRenderTargetHighRes);
	}
	else
		ChangeScopeResolution(ZoomRenderTargetLowRes);
}

void AScope::ChangeScopeResolution(UTextureRenderTarget2D* NewRenderTarget)
{
	if (DynamicScopeMaterial)
		DynamicScopeMaterial->SetTextureParameterValue(FName(TEXT("RT_Texture")), NewRenderTarget);
	
	ZoomCamera->TextureTarget = NewRenderTarget;
}
