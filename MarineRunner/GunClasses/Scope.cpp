// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/GunClasses/Scope.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/RectLightComponent.h"

// Sets default values
AScope::AScope()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick =false;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRootComponent"));
	RootComponent = SceneRootComponent;

	//Configuring the SceneCaptureComponent responsible for capturing another screen for Scope
	ZoomCamera = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("ZoomCamera"));
	ZoomCamera->SetupAttachment(RootComponent);
	ZoomCamera->FOVAngle = 15.f;
	ZoomCamera->CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR;
	//

	//Setting the crosshair to be visible in zoom mode
	Scope_Crosshair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scope_Crosshair"));
	Scope_Crosshair->SetupAttachment(ZoomCamera);
	Scope_Crosshair->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Scope_Crosshair->SetGenerateOverlapEvents(false);
	Scope_Crosshair->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Scope_Crosshair->LightingChannels.bChannel1 = true;
	//

	//Scope_Mesh that have render material from SceneCaptureComponent 
	Scope_Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scope_Mesh"));
	Scope_Mesh->SetupAttachment(RootComponent);
	Scope_Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Scope_Mesh->SetGenerateOverlapEvents(false);
	Scope_Mesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Scope_Mesh->LightingChannels.bChannel0 = false;
	Scope_Mesh->LightingChannels.bChannel1 = true;
	//

	//Scope light to prevent you from not being able to see through the scope when it's dark
	RectLightForScope = CreateDefaultSubobject<URectLightComponent>(TEXT("RectLightForScope"));
	RectLightForScope->SetupAttachment(RootComponent);
	RectLightForScope->LightingChannels.bChannel0 = false;
	RectLightForScope->LightingChannels.bChannel1 = true;
	RectLightForScope->Intensity = 100.f;
	RectLightForScope->IntensityUnits = ELightUnits::Unitless;
	RectLightForScope->AttenuationRadius = 20.f;
	RectLightForScope->BarnDoorAngle = 0.f;
	RectLightForScope->BarnDoorLength = 16.f;
	//
}

// Called when the game starts or when spawned
void AScope::BeginPlay()
{
	Super::BeginPlay();

	FOVZoom.GenerateKeyArray(FOVZoom_Keys);
	ActiveZoom(false);
}

void AScope::SetNewScope(int32 CurrentScopeIndex)
{
	if (FOVZoom.Num() < 1) return;
	
	CurrentScope = CurrentScopeIndex;
	ZoomCamera->FOVAngle = FOVZoom_Keys[CurrentScope];

	Scope_Crosshair->SetRelativeScale3D(FVector(*FOVZoom.Find(FOVZoom_Keys[CurrentScope])));
}

int32 AScope::Zoom(float WheelAxis, bool bShouldRestartScope)
{
	if (FOVZoom.Num() < 1) return 0;

	if (bShouldRestartScope == true)
	{
		SetNewScope(0);
		return 0;
	}

	if (WheelAxis > 0.1f && !(FOVZoom_Keys[FOVZoom_Keys.Num() - 1] == ZoomCamera->FOVAngle))
	{
		SetNewScope(CurrentScope + 1);
	}
	else if (WheelAxis < -0.1f && !(FOVZoom_Keys[0] == ZoomCamera->FOVAngle) && CurrentScope > 0)
	{
		SetNewScope(CurrentScope - 1);
	}
	else return CurrentScope;

	if (ZoomSound) UGameplayStatics::PlaySound2D(GetWorld(), ZoomSound);

	return CurrentScope;
}

void AScope::ActiveZoom(bool bShouldActive)
{
	if (bShouldActive)
	{
		Scope_Mesh->SetMaterial(0, RenderTargetMaterial);
		ZoomCamera->ToggleActive();
		RectLightForScope->SetVisibility(false);
	}
	else
	{
		Scope_Mesh->SetMaterial(0, ZoomMaterial);
		ZoomCamera->ToggleActive();
		RectLightForScope->SetVisibility(true);
	}
}

