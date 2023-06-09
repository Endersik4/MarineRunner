// Fill out your copyright notice in the Description page of Project Settings.


#include "DashWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

void UDashWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetMarinePawn();

}

void UDashWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	
	FadingImage();
}

void UDashWidget::FadingImage()
{
	if (MarineCamera == nullptr) return;

	if (FadeTimeElapsed < FadeTime)
	{
		float Opacity = FMath::Lerp(1.f, 0, FadeTimeElapsed / FadeTime);
		float NewFov = FMath::Lerp(Fov + StartingFOV, Fov, FadeTimeElapsed / FadeTime);
		float NewCA = FMath::Lerp(StartingChromaticAbberation, 0, FadeTimeElapsed / FadeTime);
		float NewOffsetCA = FMath::Lerp(StartingOffsetCA, 1, FadeTimeElapsed / FadeTime);

		DashImage->SetRenderOpacity(Opacity);
		if (bShouldAddFov) MarineCamera->SetFieldOfView(NewFov);

		if (bShouldAddChromaticAbberation)
		{
			MarineCamera->PostProcessSettings.ChromaticAberrationStartOffset = NewOffsetCA;
			MarineCamera->PostProcessSettings.SceneFringeIntensity = NewCA;
		}

		FadeTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	}
	else
	{
		RemoveFromParent();
	}
}

void UDashWidget::SetMarinePawn()
{
	MarineCamera = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->GetCamera();

	if (bShouldAddFov)
	{
		Fov = MarineCamera->FieldOfView;
		MarineCamera->SetFieldOfView(Fov + StartingFOV);
	}

	if (bShouldAddChromaticAbberation)
	{
		MarineCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
		MarineCamera->PostProcessSettings.bOverride_ChromaticAberrationStartOffset = true;

		MarineCamera->PostProcessSettings.ChromaticAberrationStartOffset = StartingOffsetCA;
		MarineCamera->PostProcessSettings.SceneFringeIntensity = StartingChromaticAbberation;
	}
}