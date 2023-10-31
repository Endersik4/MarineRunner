// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Objects/MessageToReadWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

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
