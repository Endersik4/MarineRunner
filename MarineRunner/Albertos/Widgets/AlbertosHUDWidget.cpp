// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Albertos/Widgets/AlbertosHUDWidget.h"
#include "Components/ProgressBar.h"

void UAlbertosHUDWidget::UpdateSprintBarValue(float NewPercent)
{
	AlbertosSprintBar->SetPercent(NewPercent);
}

void UAlbertosHUDWidget::ActivateSprintBar(bool bActivate)
{
	if (bActivate)
	{
		PlayAnimationReverse(ActivateSprintBarAnim);
	}
	else 
		PlayAnimationForward(ActivateSprintBarAnim);
}
