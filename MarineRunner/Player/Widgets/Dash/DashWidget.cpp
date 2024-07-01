// Copyright Adam Bartela.All Rights Reserved


#include "DashWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

#include "MarineRunner/Player/MarinePlayer.h"

void UDashWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UDashWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PrepareDashWidget();
}

void UDashWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	
	DashEffects(DeltaTime);
}

void UDashWidget::DashEffects(const float& Delta)
{
	if (!IsValid(PlayerCamera) || !IsValid(MarinePawn))
		return;

	if (FadeTimeElapsed <= FadeTime)
	{
		const float& Opacity = FMath::Lerp(1.f, 0, FadeTimeElapsed / FadeTime);
		const float& NewFov = FMath::Lerp(OriginalPlayerFOV + StartingFOV, OriginalPlayerFOV, FadeTimeElapsed / FadeTime);
		const float& NewCA = FMath::Lerp(StartingChromaticAbberation, OriginalChromaticAbberation, FadeTimeElapsed / FadeTime);
		const float& NewOffsetCA = FMath::Lerp(StartingOffsetCA, OriginalOffsetCA, FadeTimeElapsed / FadeTime);

		DashImage->SetRenderOpacity(Opacity);
		PlayerCamera->SetFieldOfView(NewFov);

		PlayerCamera->PostProcessSettings.ChromaticAberrationStartOffset = NewOffsetCA;
		PlayerCamera->PostProcessSettings.SceneFringeIntensity = NewCA;

		FadeTimeElapsed += Delta;
		if (FadeTimeElapsed >= FadeTime && !bRemoveDashWidget)
		{
			FadeTimeElapsed = FadeTime;
			bRemoveDashWidget = true;

			if (!MarinePawn->GetIsInSlowMotion())
				OriginalChromaticAbberation = 0.f;
		}
	}
	else
	{
		RemoveFromParent();
	}
}

void UDashWidget::PrepareDashWidget()
{
	if (!IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
		return;

	MarinePawn = Cast<AMarineCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!IsValid(MarinePawn))
		return;

	PlayerCamera = MarinePawn->GetCamera();

	OriginalPlayerFOV = PlayerCamera->FieldOfView;
	PlayerCamera->SetFieldOfView(OriginalPlayerFOV + StartingFOV);

	PlayerCamera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	PlayerCamera->PostProcessSettings.bOverride_ChromaticAberrationStartOffset = true;

	OriginalOffsetCA = PlayerCamera->PostProcessSettings.ChromaticAberrationStartOffset;
	OriginalChromaticAbberation = PlayerCamera->PostProcessSettings.SceneFringeIntensity;

	PlayerCamera->PostProcessSettings.ChromaticAberrationStartOffset = StartingOffsetCA;
	PlayerCamera->PostProcessSettings.SceneFringeIntensity = StartingChromaticAbberation;
}