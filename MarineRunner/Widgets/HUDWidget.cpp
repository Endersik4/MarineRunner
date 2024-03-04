// Copyright Adam Bartela.All Rights Reserved


#include "MarineRunner/Widgets/HUDWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	WhichElementShouldProgress(DeltaTime);
}

void UHUDWidget::SetHealthBarPercent(float CurrentHealth)
{
	float Health = CurrentHealth / 100.f;
	HealthBar->SetPercent(Health);
}

void UHUDWidget::SetCurrentNumberOfFirstAidKits(int32 CurrentAidKitsNumber)
{
	FString CurrentNumberString = FString::FromInt(CurrentAidKitsNumber);
	if (CurrentAidKitsNumber < 10)
	{
		CurrentNumberString = "0" + FString::FromInt(CurrentAidKitsNumber);
	}
	CurrentNumbersOfFirstAidKit->SetText(FText::FromString(CurrentNumberString));
}

#pragma region //////////////////// WEAPON UI //////////////////////////
void UHUDWidget::SetAmmoText(int32 Ammo, bool bSetStoredAmmo)
{
	FString AmmoText = "";
	if (Ammo > 999)
		AmmoText = "+999";
	else
	{
		if (Ammo < 10) AmmoText = "00";
		else if (Ammo < 100) AmmoText = "0";
		AmmoText += FString::FromInt(Ammo);
	}

	if (bSetStoredAmmo)
	{
		StoredAmmoText->SetText(FText::FromString(AmmoText));
	}
	else CurrentAmmoInMagazineText->SetText(FText::FromString(AmmoText));
}

void UHUDWidget::SetWeaponImage(UTexture2D* Texture, bool bAmmoCounterBelowGunHUD)
{
	if (bAmmoCounterBelowGunHUD == true) WeaponImage->SetDesiredSizeOverride(WeaponImageSizeWhenAmmoBelow);
	else WeaponImage->SetDesiredSizeOverride(WeaponImageSizeWhenAmmoOnSide);
	
	WeaponImage->SetBrushFromTexture(Texture, true);
}

void UHUDWidget::ShowWeaponOnHud(bool bShow)
{
	if (WeaponAppearAnim == nullptr) return;

	if (bShow == true) PlayAnimationForward(WeaponAppearAnim);
	else PlayAnimationReverse(WeaponAppearAnim);
}
#pragma endregion

#pragma region ////////////// FILL PROGRESS BARS //////////////////
void UHUDWidget::WhichElementShouldProgress(float Delta)
{
	if (bShouldProgress == false || UGameplayStatics::IsGamePaused(GetWorld()) == true) 
		return;

	ProgressBarForUseableElements(HealBar, EUseableElement::Heal, ActiveHealAnim, Delta);
	ProgressBarForUseableElements(DashBar, EUseableElement::Dash, ActiveDashAnim, Delta);
	ProgressBarForUseableElements(SlowMoBar, EUseableElement::SlowMo, ActiveSlowMotionAnim, Delta);
}

void UHUDWidget::ProgressBarForUseableElements(UProgressBar* ProgressBarElement, EUseableElement Element, UWidgetAnimation* AnimToPlayAfterFinish, float Delta)
{
	if (!WhichElementToProgress.Contains(Element)) return;

	if (WhichElementToProgress[Element].ProgressTimeElapsed <= WhichElementToProgress[Element].MaxToZeroTime)
	{
		float NewPercent = FMath::Lerp(1.f, 0.f, WhichElementToProgress[Element].ProgressTimeElapsed / WhichElementToProgress[Element].MaxToZeroTime);

		ProgressBarElement->SetPercent(NewPercent);
		WhichElementToProgress[Element].ProgressTimeElapsed += Delta;

		return;
	}
	else
	{
		if (PowerUpLoadedSound) UGameplayStatics::SpawnSound2D(GetWorld(), PowerUpLoadedSound);
		PlayAnimationForward(AnimToPlayAfterFinish);

		ProgressBarElement->SetPercent(0.f);
		WhichElementToProgress.Remove(Element);
		if (WhichElementToProgress.Num() <= 0) bShouldProgress = false;
	}
}

void UHUDWidget::AddElementToProgress(EUseableElement Element, ElementBar ElementProgressBar)
{
	WhichElementToProgress.Add(Element, ElementProgressBar);
	bShouldProgress = true;
}
#pragma endregion

#pragma region ////////////// ANIMATIONS WIDGETS //////////////////
void UHUDWidget::PlayAppearAnimForItemHover(bool bForwardAnim)
{
	if (ItemHoverAppearAnim == nullptr || ItemHoverName->GetText().ToString() == "") return;

	if (bForwardAnim) PlayAnimationForward(ItemHoverAppearAnim);
	else PlayAnimationReverse(ItemHoverAppearAnim);
}

void UHUDWidget::PlayUseFirstAidKitAnim()
{
	if (HealthBar->GetPercent() > 0.2f)
	{
		PlayAnimationForward(UseFirstAidKitAnim);

	}
	else
		PlayAnimationForward(UseFirstAidKitLessHPAnim);
	
}

void UHUDWidget::PlayGotDamageAnim()
{
	if (HealthBar->GetPercent() > 0.2f)
	{
		PlayAnimationForward(GotDamageAnim);

	}
	else
		PlayAnimationForward(GotDamageLessHPAnim);
}

void UHUDWidget::PlayButtonAnimation(EAnimationToPlay AnimToPlay)
{
	if (AnimToPlay == EATP_PressedButton_Heal)
	{
		PlayAnimationForward(PressedHealButtonAnim);
	}
	else if (AnimToPlay == EATP_PressedButton_Dash)
	{
		PlayAnimationForward(PressedDashButtonAnim);
	}
	else if (AnimToPlay == EATP_PressedButton_SlowMo)
	{
		PlayAnimationForward(PressedSlowMotionButtonAnim);
	}
}
#pragma endregion

void UHUDWidget::SetItemHoverInformations(const FString& ItemName, const FString& ItemDescription, UTexture2D* ItemIcon)
{
	PlayAppearAnimForItemHover();

	ItemHoverName->SetText(FText::FromString("-" + ItemName + "-"));
	ItemHoverDescription->SetText(FText::FromString(ItemDescription));
	ItemHoverImage->SetBrushFromTexture(ItemIcon);
}

void UHUDWidget::ShowGameplayMechanicsBars(bool bUnlockHeal, bool bUnlockDash, bool bUnlockSlowMo)
{
	if (bUnlockHeal)
		PlayAnimationForward(ShowHealBarAnim);
	if (bUnlockDash)
		PlayAnimationForward(ShowDashBarAnim);
	if (bUnlockSlowMo)
		PlayAnimationForward(ShowSlowMotionBarAnim);
}

void UHUDWidget::ShowGameplayMechanicsBars(const EUnlockInHud& WhatToUnlock)
{
	if (WhatToUnlock == EUnlockInHud::EUIN_HealBar)
		PlayAnimationForward(ShowHealBarAnim);
	else if (WhatToUnlock == EUnlockInHud::EUIN_DashBar)
		PlayAnimationForward(ShowDashBarAnim);
	else if (WhatToUnlock == EUnlockInHud::EUIN_SlowMoBar)
		PlayAnimationForward(ShowSlowMotionBarAnim);
}