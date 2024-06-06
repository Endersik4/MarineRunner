// Copyright Adam Bartela.All Rights Reserved


#include "HUDWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHUDWidget::NativeOnInitialized()
{
	if (HealthBar->GetPercent() < MinHPToIndicateLowHP)
	{
		PlayAnimationForward(UseFirstAidKitLessHPAnim);
	}
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	ProgressPowerUps(DeltaTime / DeltaDivider);
}

void UHUDWidget::ShowHUDWithAnim(bool bShow)
{
	if (bShow)
		PlayAnimationForward(ShowHUDWidget);
	else
		PlayAnimationForward(HideHUDWidget);
}

void UHUDWidget::UpdateHealthBarPercent(float CurrentHealth)
{
	const float& HealthToPercent = 100.f;
	const float& Health = CurrentHealth / HealthToPercent;
	HealthBar->SetPercent(Health);
}

void UHUDWidget::UpdateCurrentNumberOfFirstAidKits(int32 CurrentAidKitsNumber)
{
	const FString& FirstAidKitAmountString = CurrentAidKitsNumber < 10 ? "0" + FString::FromInt(CurrentAidKitsNumber) : FString::FromInt(CurrentAidKitsNumber);
	CurrentNumbersOfFirstAidKit->SetText(FText::FromString(FirstAidKitAmountString));
}

#pragma region //////////////////// WEAPON UI //////////////////////////
FText UHUDWidget::AmmoValueToText(const int32& AmmoAmountToText)
{
	FString AmmoText = "";
	if (AmmoAmountToText > 999)
	{
		AmmoText = "+999";
	}
	else
	{
		if (AmmoAmountToText < 10)
			AmmoText = "00";
		else if (AmmoAmountToText < 100)
			AmmoText = "0";

		AmmoText += FString::FromInt(AmmoAmountToText);
	}

	return FText::FromString(AmmoText);
}

void UHUDWidget::UpdateAmmoInMagazineAmountText(const int32& Ammo)
{
	CurrentAmmoInMagazineText->SetText(AmmoValueToText(Ammo));
}

void UHUDWidget::UpdateStoredAmmoAmountText(const int32& Ammo)
{
	StoredAmmoText->SetText(AmmoValueToText(Ammo));
}

void UHUDWidget::UpdateWeaponImage(TObjectPtr<UTexture2D> Texture, bool bAmmoCounterBelowGunHUD)
{
	if (bAmmoCounterBelowGunHUD) 
		WeaponImage->SetDesiredSizeOverride(WeaponImageSizeWhenAmmoBelow);
	else 
		WeaponImage->SetDesiredSizeOverride(WeaponImageSizeWhenAmmoOnSide);
	
	WeaponImage->SetBrushFromTexture(Texture, true);
}

void UHUDWidget::ShowWeaponOnHud(bool bShow)
{
	if (!WeaponAppearAnim) 
		return;

	if (bShow) 
		PlayAnimationForward(WeaponAppearAnim);
	else 
		PlayAnimationReverse(WeaponAppearAnim);
}
#pragma endregion

#pragma region ////////////// FILL PROGRESS BARS //////////////////
void UHUDWidget::AddNewPowerUpToStartLoading(const FPowerUpLoaded& NewPowerUpToAdd)
{
	PowerUpsToLoad.Remove(NewPowerUpToAdd);
	PowerUpsToLoad.Add(NewPowerUpToAdd);
}

void UHUDWidget::ProgressPowerUps(const float & Delta)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()) || PowerUpsToLoad.Num() == 0)
		return;

	for (FPowerUpLoaded& CurrentPowerUp : PowerUpsToLoad)
	{
		LoadingPowerUp(CurrentPowerUp, Delta);
	}
	for (FPowerUpLoaded& CurrentPowerUp : PowerUpsToDelete)
	{
		PowerUpsToLoad.Remove(CurrentPowerUp);
	}

	if (PowerUpsToDelete.Num() > 0)
		PowerUpsToDelete.Empty();
}

void UHUDWidget::LoadingPowerUp(FPowerUpLoaded& PowerUpToLoad, const float & Delta)
{
	if (!PowerUpToLoad.bStartPowerUpLoading)
		return;

	if (PowerUpToLoad.PowerUpFillTimeElapsed <= PowerUpToLoad.TimeToFillPowerUp)
	{
		const float& PowerUpFillValue = FMath::Lerp(1.f, 0.f, PowerUpToLoad.PowerUpFillTimeElapsed / PowerUpToLoad.TimeToFillPowerUp);
		PowerUpToLoad.PowerUpProgressBarToFill->SetPercent(PowerUpFillValue);

		PowerUpToLoad.PowerUpFillTimeElapsed += Delta;
	}
	else
	{
		if (IsValid(PowerUpLoadedSound))
			UGameplayStatics::SpawnSound2D(GetWorld(), PowerUpLoadedSound);
		else
			UE_LOG(LogTemp, Warning, TEXT("Power Up Loaded Sound is nullptr in HUDWidget!"));

		PlayAnimationForward(PowerUpToLoad.PowerUpAnimToPlayAfterFill);

		PowerUpToLoad.PowerUpFillTimeElapsed = 0.f;
		PowerUpToLoad.PowerUpProgressBarToFill->SetPercent(0.f);
		PowerUpToLoad.bStartPowerUpLoading = false;

		PowerUpsToDelete.Add(PowerUpToLoad);
	}
}
#pragma endregion

#pragma region ////////////// ANIMATIONS WIDGETS //////////////////
void UHUDWidget::PlayAppearAnimForItemHover(bool bForwardAnim)
{
	if (!ItemHoverAppearAnim || ItemHoverName->GetText().ToString() == "") 
		return;

	if (bForwardAnim) 
		PlayAnimationForward(ItemHoverAppearAnim);
	else 
		PlayAnimationReverse(ItemHoverAppearAnim);
}

void UHUDWidget::PlayUseFirstAidKitAnim()
{
	if (HealthBar->GetPercent() > MinHPToIndicateLowHP)
	{
		PlayAnimationForward(UseFirstAidKitAnim);
	}
	else
	{
		PlayAnimationForward(UseFirstAidKitLessHPAnim);
	}
}

void UHUDWidget::PlayGotDamageAnim()
{
	if (HealthBar->GetPercent() > MinHPToIndicateLowHP)
	{
		PlayAnimationForward(GotDamageAnim);

	}
	else
	{
		PlayAnimationForward(GotDamageLessHPAnim);
	}
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

void UHUDWidget::SetItemHoverInformations(const FString& ItemName, const FString& ItemDescription, TObjectPtr<UTexture2D> ItemIcon)
{
	PlayAppearAnimForItemHover();

	ItemHoverName->SetText(FText::FromString("-" + ItemName + "-"));
	ItemHoverDescription->SetText(FText::FromString(ItemDescription));
	ItemHoverImage->SetBrushFromTexture(ItemIcon);
}

void UHUDWidget::ShowGameplayMechanicsBars(const bool bUnlockHeal, const bool bUnlockDash, const bool bUnlockSlowMo)
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