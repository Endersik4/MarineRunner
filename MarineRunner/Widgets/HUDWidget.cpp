// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/HUDWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "MarineRunner/MarinePawnClasses/MarineCharacter.h"
#include "MarineRunner/Inventory/InventoryComponent.h"

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
	FadeFirstAidImage();
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
	int32 CurrentNumber = MarinePawn->GetInventoryComponent()->Inventory_Items.Find(MarinePawn->GetFirstAidKitName())->Item_Amount;
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

void UHUDWidget::SetWeaponImage(UTexture2D* Texture, bool bAmmoCounterBelowGunHUD)
{
	if (bAmmoCounterBelowGunHUD == true) WeaponImage->SetDesiredSizeOverride(FVector2D(460.f, 260.f));
	else WeaponImage->SetDesiredSizeOverride(FVector2D(260.f, 150.f));
	
	WeaponImage->SetBrushFromTexture(Texture, true);
}

void UHUDWidget::SetGotDamage(bool bGot)
{
	bGotDamage = bGot;
	if (bGotDamage)
	{
		GotDamageImage->SetRenderOpacity(1.f);
		FadeGotDamageTimeElapsed = 0.f;
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

	if (FadeGotDamageTimeElapsed <= FadeGotDamageTime)
	{
		float Opacity = FMath::Lerp(1.f, 0, FadeGotDamageTimeElapsed / FadeGotDamageTime);

		GotDamageImage->SetRenderOpacity(Opacity);
		FadeGotDamageTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	}
	else
	{
		GotDamageImage->SetRenderOpacity(0.f);
		bGotDamage = false;
	}
}

void UHUDWidget::FadeFirstAidImage()
{
	if (bDidPlayerUseFirstAidKit == false) return;

	if (FadeFirstAidImageTimeElapsed <= FadeFirstAidImageTime)
	{
		float Opacity = FMath::Lerp(1.f, 0, FadeFirstAidImageTimeElapsed / FadeFirstAidImageTime);

		UseFirstAidKidImage->SetRenderOpacity(Opacity);
		FadeFirstAidImageTimeElapsed += UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	}
	else
	{
		FadeFirstAidImageTimeElapsed = 0.f;
		UseFirstAidKidImage->SetRenderOpacity(0.f);
		bDidPlayerUseFirstAidKit = false;
	}
}


void UHUDWidget::WhichElementShouldProgress()
{
	if (bShouldProgress == false) return;

	ProgressBarForUseableElements(HealBar, EUseableElement::Heal, true);
	ProgressBarForUseableElements(DashBar, EUseableElement::Dash, true);
	ProgressBarForUseableElements(SlowMoBar, EUseableElement::SlowMo, true);
	ProgressBarForUseableElements(Button_HealBar, EUseableElement::Button_Heal);
	ProgressBarForUseableElements(Button_DashBar, EUseableElement::Button_Dash);
	ProgressBarForUseableElements(Button_SlowMoBar, EUseableElement::Button_SlowMo);
}

void UHUDWidget::ProgressBarForUseableElements(UProgressBar* ProgressBarElement, EUseableElement Element, bool bShouldAddSound)
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
		if (PowerUpLoadedSound && bShouldAddSound) UGameplayStatics::SpawnSound2D(GetWorld(), PowerUpLoadedSound);
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

void UHUDWidget::HideItemHover(ESlateVisibility NewVisibility)
{
	ItemHoverImage->SetVisibility(NewVisibility);
	ItemHoverName->SetVisibility(NewVisibility);
	ItemHoverDescription->SetVisibility(NewVisibility);
}

void UHUDWidget::SetItemHoverStuff(FItemStruct ItemStruct)
{
	HideItemHover(ESlateVisibility::Visible);

	ItemHoverName->SetText(FText::FromString(ItemStruct.Item_Name));
	ItemHoverDescription->SetText(FText::FromString(ItemStruct.Item_Description));
}

void UHUDWidget::SetUpMarinePawn()
{
	MarinePawn = Cast<AMarineCharacter>(GetOwningPlayerPawn());

	SetHealthPercent();
	SetCurrentNumberOfFirstAidKits();
}