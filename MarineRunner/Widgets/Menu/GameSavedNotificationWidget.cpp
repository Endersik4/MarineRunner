// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/GameSavedNotificationWidget.h"
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
