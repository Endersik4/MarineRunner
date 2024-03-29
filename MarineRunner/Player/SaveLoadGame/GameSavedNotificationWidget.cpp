// Copyright Adam Bartela.All Rights Reserved


#include "GameSavedNotificationWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UGameSavedNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGameSavedNotificationWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FWidgetAnimationDynamicEvent GameSavedDynamicEvent;
	GameSavedDynamicEvent.BindDynamic(this, &UGameSavedNotificationWidget::GameSavedAnimFinished);
	BindToAnimationFinished(GameSavedAppearAnim, GameSavedDynamicEvent);

	PlayAnimationForward(GameSavedAppearAnim);
}

void UGameSavedNotificationWidget::GameSavedAnimFinished()
{
	RemoveFromParent();
}
