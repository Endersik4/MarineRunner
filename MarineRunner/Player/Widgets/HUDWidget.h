// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
enum EAnimationToPlay
{
	EATP_PressedButton_Heal,
	EATP_PressedButton_Dash,
	EATP_PressedButton_SlowMo,
};

USTRUCT(BlueprintType)
struct FPowerUpLoaded
{
	GENERATED_USTRUCT_BODY();

	bool bStartPowerUpLoading = false;
	float TimeToFillPowerUp = 0.f;
	float PowerUpFillTimeElapsed = 0.f;
	UPROPERTY(Transient)
		TObjectPtr<UWidgetAnimation> PowerUpAnimToPlayAfterFill;
	UPROPERTY(Transient)
		TObjectPtr<UProgressBar> PowerUpProgressBarToFill;

	FPowerUpLoaded() 
	{
		bStartPowerUpLoading = false;
		TimeToFillPowerUp = 0.f;
		PowerUpFillTimeElapsed = 0.f;
		PowerUpAnimToPlayAfterFill = nullptr;
		PowerUpProgressBarToFill = nullptr;
	}

	FPowerUpLoaded(bool _bStartPowerUpLoading, float _TimeToFillPowerUp, TObjectPtr<UWidgetAnimation> _PowerUpAnimToPlayAfterFill, TObjectPtr<UProgressBar> _PowerUpProgressBarToFill)
	{
		bStartPowerUpLoading = _bStartPowerUpLoading;
		TimeToFillPowerUp = _TimeToFillPowerUp;
		PowerUpAnimToPlayAfterFill = _PowerUpAnimToPlayAfterFill;
		PowerUpProgressBarToFill = _PowerUpProgressBarToFill;
	}

	bool operator==(const FPowerUpLoaded& _PowerUp)
	{
		return PowerUpProgressBarToFill == _PowerUp.PowerUpProgressBarToFill;
	}
};

UENUM(BlueprintType)
enum EUnlockInHud
{
	EUIN_HealBar,
	EUIN_DashBar,
	EUIN_SlowMoBar,
};

UCLASS()
class MARINERUNNER_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	void UpdateHealthBarPercent(float CurrentHealth);
	void UpdateCurrentNumberOfFirstAidKits(int32 CurrentAidKitsNumber);

	void UpdateAmmoInMagazineAmountText(const int32 &Ammo);
	void UpdateStoredAmmoAmountText(const int32 &Ammo);
	void UpdateWeaponImage(TObjectPtr<UTexture2D> Texture, bool bAmmoCounterBelowGunHUD);

	void PlayGotDamageAnim();
	void PlayUseFirstAidKitAnim();
	void PlayButtonAnimation(EAnimationToPlay AnimToPlay);

	void ShowWeaponOnHud(bool bShow = true);

	void AddNewPowerUpToStartLoading(const FPowerUpLoaded & NewPowerUpToAdd);

	UFUNCTION(BlueprintImplementableEvent)
		void NewGameStartedWidgetAnimation();

	void ShowGameplayMechanicsBars(const bool bUnlockHeal, const bool bUnlockDash, const bool bUnlockSlowMo);
	void ShowGameplayMechanicsBars(const EUnlockInHud& WhatToUnlock);

#pragma region ///////////// USE FIRST AID KIT UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> UseFirstAidKidImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> UseFirstAidKitAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> UseFirstAidKitLessHPAnim = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> HealthBar = nullptr;
#pragma endregion

#pragma region ///////////// HEAL UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> HealBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> Button_HealBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> KeyText_Heal = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> CurrentNumbersOfFirstAidKit = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> PressedHealButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ActiveHealAnim = nullptr;
#pragma endregion

#pragma region ///////////// DASH UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> DashBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> Button_DashBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> KeyText_Dash = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> PressedDashButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ActiveDashAnim = nullptr;
#pragma endregion

#pragma region ///////////// SLOW MOTION UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> SlowMoBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UProgressBar> Button_SlowMoBar = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> KeyText_SlowMo = nullptr;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> PressedSlowMotionButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ActiveSlowMotionAnim = nullptr;
#pragma endregion

#pragma region ///////////// GOT DAMAGE UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> GotDamageImage;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> GotDamageAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> GotDamageLessHPAnim = nullptr;
#pragma endregion

#pragma region ///////////// WEAPON UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> WeaponImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> WeaponAppearAnim = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> CurrentAmmoInMagazineText = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> StoredAmmoText = nullptr;
#pragma endregion

#pragma region ///////////// ITEM HOVER UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> ItemHoverBackgroundImage = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemHoverName = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UTextBlock> ItemHoverDescription = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		TObjectPtr<UImage> ItemHoverImage = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ItemHoverAppearAnim = nullptr;
#pragma endregion

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ShowHealBarAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ShowDashBarAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		TObjectPtr<UWidgetAnimation> ShowSlowMotionBarAnim = nullptr;

	// On Item Hover
	void PlayAppearAnimForItemHover(bool bForwardAnim = true);
	void SetItemHoverInformations(const FString& ItemName, const FString& ItemDescription, TObjectPtr < UTexture2D> ItemIcon);
private:
	// in percent
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
		float MinHPToIndicateLowHP = 0.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Player")
		TObjectPtr<USoundBase> PowerUpLoadedSound = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FVector2D WeaponImageSizeWhenAmmoBelow = FVector2D(460.f, 260.f);
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FVector2D WeaponImageSizeWhenAmmoOnSide = FVector2D(260.f, 150.f);

	FText AmmoValueToText(const int32& AmmoAmountToText);

	UPROPERTY(Transient)
		TArray<FPowerUpLoaded> PowerUpsToLoad;
	UPROPERTY(Transient)
		TArray<FPowerUpLoaded> PowerUpsToDelete;

	void ProgressPowerUps(const float & Delta);
	void LoadingPowerUp(FPowerUpLoaded & PowerUpToLoad, const float & Delta);
};
