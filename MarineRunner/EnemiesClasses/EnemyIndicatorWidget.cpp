// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/EnemiesClasses/EnemyIndicatorWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UEnemyIndicatorWidget::SetCurrentHealthInHealthBar(float NewHealth)
{
	if (NewHealth < 0.f)
		return;

	CurrentHealthBar->SetPercent(NewHealth / MaxHealth);
	GetWorld()->GetTimerManager().SetTimer(DelayedHealthProgressBarHandle, this, &UEnemyIndicatorWidget::ApplyCurrentHealthToDelayedBar, DelayedHealthTime, false);
}

void UEnemyIndicatorWidget::ApplyCurrentHealthToDelayedBar()
{
	DelayedHealthBar->SetPercent(CurrentHealthBar->GetPercent());
}