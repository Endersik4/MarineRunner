// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/HUDWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHUDWidget::NativeOnInitialized()
{
	SetUpMarinePawn();

	GotDamageImage->SetRenderOpacity(0.f);
	HideWeaponThings(true);
}

void UHUDWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	FadeGotDamageImage();
	WhichElementShouldProgress();
}

void UHUDWidget::SetHealthPercent()
{
	if (!MarinePawn) return;

	float Health = MarinePawn->GetHealth() / 100.f;
	HealthBar->SetPercent(Health);
}

void UHUDWidget::SetCurrentNumberOfFirstAidKits()
{
	int32 CurrentNumber = MarinePawn->GetFirstAidKits();
	FString CurrentNumberString = FString::FromInt(CurrentNumber);
	if (CurrentNumber < 10)
	{
		CurrentNumberString = "0" + FString::FromInt(CurrentNumber);
	}
	CurrentNumbersOfFirstAidKit->SetText(FText::FromString(CurrentNumberString));
}

void UHUDWidget::SetAmmoText(int32 Ammo, bool bSetStoredAmmo)
{
	FString AmmoText = "";
	if (Ammo < 10) AmmoText = "00";
	else if (Ammo < 100) AmmoText = "0";
	AmmoText += FString::FromInt(Ammo);

	if (bSetStoredAmmo)
	{
		StoredAmmoText->SetText(FText::FromString(AmmoText));
	}
	else CurrentAmmoInMagazineText->SetText(FText::FromString(AmmoText));
}

void UHUDWidget::SetWeaponImage(UTexture2D* Texture)
{
	WeaponImage->SetBrushFromTexture(Texture, true);
}

void UHUDWidget::SetGotDamage(bool bGot)
{
	bGotDamage = bGot;
	if (bGotDamage)
	{
		GotDamageImage->SetRenderOpacity(1.f);
		FadeTimeElapsed = 0.f;
	}
	else GotDamageImage->SetRenderOpacity(0.f);
}

void UHUDWidget::HideWeaponThings(bool bShouldHide)
{
	float RenderOpacityValue = 0.f;
	if (bShouldHide == false) RenderOpacityValue = 1.f;

	StoredAmmoText->SetRenderOpacity(RenderOpacityValue);
	CurrentAmmoInMagazineText->SetRenderOpacity(RenderOpacityValue);
	WeaponImage->SetRenderOpacity(RenderOpacityValue);
}

void UHUDWidget::FadeGotDamageImage()
{
	if (!bGotDamage) return;

	if (FadeTimeElapsed < FadeTime)
	{
		float Opacity = FMath::Lerp(1.f, 0, FadeTimeElapsed / FadeTime);

		GotDamageImage->SetRenderOpacity(Opacity);
		FadeTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	}
}

void UHUDWidget::WhichElementShouldProgress()
{
	if (bShouldProgress == false) return;

	ProgressBarForUseableElements(HealBar, EUseableElement::Heal);
	ProgressBarForUseableElements(DashBar, EUseableElement::Dash);
	ProgressBarForUseableElements(SlowMoBar, EUseableElement::SlowMo);
	ProgressBarForUseableElements(Button_HealBar, EUseableElement::Button_Heal);
	ProgressBarForUseableElements(Button_DashBar, EUseableElement::Button_Dash);
	ProgressBarForUseableElements(Button_SlowMoBar, EUseableElement::Button_SlowMo);
}

void UHUDWidget::ProgressBarForUseableElements(UProgressBar* ProgressBarElement, EUseableElement Element)
{
	if (!WhichElementToProgress.Contains(Element)) return;

	if (WhichElementToProgress[Element].ProgressTimeElapsed <= WhichElementToProgress[Element].MaxToZeroTime)
	{
		float NewPercent = FMath::Lerp(1.f, 0.f, WhichElementToProgress[Element].ProgressTimeElapsed / WhichElementToProgress[Element].MaxToZeroTime);

		ProgressBarElement->SetPercent(NewPercent);
		WhichElementToProgress[Element].ProgressTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		return;
	}
	else
	{
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

void UHUDWidget::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(GetOwningPlayerPawn());

	SetHealthPercent();
	SetCurrentNumberOfFirstAidKits();
}
