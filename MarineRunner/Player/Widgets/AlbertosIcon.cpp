// Copyright Adam Bartela.All Rights Reserved

#include "MarineRunner/Player/Widgets/AlbertosIcon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "MarineRunner/Albertos/AlbertosPawn.h"

void UAlbertosIcon::StartTrackingAlbertos()
{
	if (!IsValid(AlbertosPawn))
		return;

	AlbertosIconPanelSlot = Cast<UCanvasPanelSlot>(AlbertosIconImage->Slot);

	ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());

	GetWorld()->GetTimerManager().SetTimer(TrackAlbertosLocationHandle, this, &UAlbertosIcon::UpdateAlbertosIconPosition, TrackAlbertosLocationTime, true, 0.f);
}

void UAlbertosIcon::StopTrackingAlbertos()
{
	GetWorld()->GetTimerManager().ClearTimer(TrackAlbertosLocationHandle);
}

void UAlbertosIcon::UpdateAlbertosIconPosition()
{
	if (!IsValid(AlbertosPawn) || !IsValid(AlbertosIconPanelSlot))
		return;

	const FVector HeadLocation = AlbertosPawn->GetAlbertosSkeletal()->GetSocketLocation(IconLocationSocketName);
	FVector2D AlbertosIconScreenPosition = FVector2D(0.f);

	bool bProjected = UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), HeadLocation, AlbertosIconScreenPosition, true);

	if (bProjected)
	{
		AlbertosIconScreenPosition.X = FMath::Clamp(AlbertosIconScreenPosition.X, 0.f, ViewportSize.X);
		AlbertosIconScreenPosition.Y = FMath::Clamp(AlbertosIconScreenPosition.Y, 0.f, ViewportSize.Y);
	}
	else
		AlbertosIconScreenPosition.Y = ViewportSize.Y / 2.f;

	AlbertosIconPanelSlot->SetPosition(AlbertosIconScreenPosition);
}
