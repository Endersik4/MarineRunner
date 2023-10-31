// Fill out your copyright notice in the Description page of Project Settings.


#include "DashWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

void UDashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PrepareDashWidget();
}

void UDashWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	
	DashEffects(DeltaTime);
}

void UDashWidget::DashEffects(float Delta)
{
	if (MarineCamera == nullptr) return;

	if (FadeTimeElapsed <= FadeTime)
	{
		float Opacity = FMath::Lerp(1.f, 0, FadeTimeElapsed / FadeTime);
		float NewFov = FMath::Lerp(OriginalPlayerFOV + StartingFOV, OriginalPlayerFOV, FadeTimeElapsed / FadeTime);
		float NewCA = FMath::Lerp(StartingChromaticAbberation, OriginalChromaticAbberation, FadeTimeElapsed / FadeTime);
		float NewOffsetCA = FMath::Lerp(StartingOffsetCA, OriginalOffsetCA, FadeTimeElapsed / FadeTime);

		DashImage->SetRenderOpacity(Opacity);
		MarineCamera->SetFieldOfView(NewFov);

		MarineCamera->PostProcessSettings.ChromaticAberrationStartOffset = NewOffsetCA;
		MarineCamera->PostProcessSettings.SceneFringeIntensity = NewCA;

		FadeTimeElapsed += Delta;
		if (FadeTimeElapsed >= FadeTime) FadeTimeElapsed = FadeTime;
	}
	else
	{
		RemoveFromParent();
	}
}

void UDashWidget::PrepareDashWidget()
{
	MarineCamera = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->GetCamera();

	OriginalPlayerFOV = MarineCamera->FieldOfView;
	MarineCamera->SetFieldOfView(OriginalPlayerFOV + StartingFOV);

	MarineCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	MarineCamera->PostProcessSettings.bOverride_ChromaticAberrationStartOffset = true;

	OriginalOffsetCA = MarineCamera->PostProcessSettings.ChromaticAberrationStartOffset;
	OriginalChromaticAbberation = MarineCamera->PostProcessSettings.SceneFringeIntensity;

	MarineCamera->PostProcessSettings.ChromaticAberrationStartOffset = StartingOffsetCA;
	MarineCamera->PostProcessSettings.SceneFringeIntensity = StartingChromaticAbberation;
}