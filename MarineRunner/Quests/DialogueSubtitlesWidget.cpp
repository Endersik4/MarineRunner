// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Quests/DialogueSubtitlesWidget.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Animation/WidgetAnimation.h"
void UDialogueSubtitlesWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDialogueSubtitlesWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ShowWidget(true);

	FWidgetAnimationDynamicEvent ShowWidgetAnimFinishedDelegate;
	ShowWidgetAnimFinishedDelegate.BindDynamic(this, &UDialogueSubtitlesWidget::RemoveFromParent);

	BindToAnimationFinished(HideWidgetAnim, ShowWidgetAnimFinishedDelegate);
}

void UDialogueSubtitlesWidget::ChangeCurrentSubtitles(const FText& NewTextForSubtitles)
{
	CurrentSubtitlesRichText->SetText(NewTextForSubtitles);
}

void UDialogueSubtitlesWidget::ShowWidget(bool bShow)
{
	if (bShow)
	{
		PlayAnimationForward(ShowWidgetAnim);
	}
	else
	{
		PlayAnimationForward(HideWidgetAnim);
	}
}
