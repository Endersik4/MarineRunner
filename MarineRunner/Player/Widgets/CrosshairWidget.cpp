// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Player/Widgets/CrosshairWidget.h"
#include "Components/Image.h"

void UCrosshairWidget::ChangeCrosshair(TObjectPtr<UTexture2D> NewCrosshairTexture)
{
	if (!IsValid(NewCrosshairTexture))
		return;

	CrosshairImage->SetBrushFromTexture(NewCrosshairTexture);
}