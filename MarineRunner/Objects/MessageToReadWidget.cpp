// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"

void UMessageToReadWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayAnimationForward(OpenMessageAnim, 1.f, true);
	if (OpenMessageSound) UGameplayStatics::PlaySound2D(GetWorld(), OpenMessageSound);
}

void UMessageToReadWidget::SetMessageInformation(const FText& _MessageTitle, const FText& _MessageText)
{
	MessageTitle->SetText(_MessageTitle);
	MessageText->SetText(_MessageText);
}

void UMessageToReadWidget::HideMessageAfterTime(float Time)
{
	GetWorld()->GetTimerManager().SetTimer(HideMessageHandle, this, &UMessageToReadWidget::HideMessage, Time, false);
}

void UMessageToReadWidget::HideMessage()
{
	PlayAnimationReverse(OpenMessageAnim, 1.f, true);
	
	FTimerHandle RemoveFromViewportHandle;
	GetWorld()->GetTimerManager().SetTimer(RemoveFromViewportHandle, this, &UMessageToReadWidget::RemoveFromParent, OpenMessageAnim->GetEndTime(), false);

}
