// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

UENUM()
enum EUseableElement
{
	Heal,
	Dash,
	SlowMo,
	Button_Heal,
	Button_Dash,
	Button_SlowMo,
};

struct ElementBar
{
	float MaxToZeroTime;
	float ProgressTimeElapsed = 0.f;
};

/**
 *
 */
UCLASS()
class MARINERUNNER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeOnInitialized() override; 
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	void SetHealthPercent();
	void SetCurrentNumberOfFirstAidKits();
	//if bSetStoredAmmo == false then set ammo text in CurrentAmmoInMagazineText
	//otherwise set ammo in StoredAmmoText
	void SetAmmoText(int32 Ammo, bool bSetStoredAmmo = false);
	void SetWeaponImage(UTexture2D* Texture);
	void SetGotDamage(bool bGot);
	void SetDidPlayerUseFirstAidKit(bool bDid) { bDidPlayerUseFirstAidKit = bDid; }
	void AddElementToProgress(EUseableElement Element, ElementBar ElementProgressBar);

	void HideWeaponThings(bool bShouldHide);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* UseFirstAidKidImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* WeaponImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HealBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* DashBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* SlowMoBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_HealBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_DashBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_SlowMoBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentNumbersOfFirstAidKit;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_Heal;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_Dash;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_SlowMo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentAmmoInMagazineText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* StoredAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* GotDamageImage;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeGotDamageTime = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeFirstAidImageTime = 0.8f;

	bool bGotDamage;
	float FadeGotDamageTimeElapsed;
	void FadeGotDamageImage();

	bool bDidPlayerUseFirstAidKit;
	float FadeFirstAidImageTimeElapsed;
	void FadeFirstAidImage();

	bool bShouldProgress;
	TMap<EUseableElement, ElementBar> WhichElementToProgress;
	void WhichElementShouldProgress();
	void ProgressBarForUseableElements(class UProgressBar* ProgressBarElement, EUseableElement Element);

	void SetUpMarinePawn();
	class AMarineCharacter* MarinePawn;
};
