// Copyright Adam Bartela.All Rights Reserved

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

enum EAnimationToPlay
{
	EATP_PressedButton_Heal,
	EATP_PressedButton_Dash,
	EATP_PressedButton_SlowMo,
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

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

public:
	void SetHealthBarPercent(float CurrentHealth);
	void SetCurrentNumberOfFirstAidKits(int32 CurrentAidKitsNumber);

	//if bSetStoredAmmo == false then set ammo text in CurrentAmmoInMagazineText
	//otherwise set ammo in StoredAmmoText
	void SetAmmoText(int32 Ammo, bool bSetStoredAmmo = false);
	void SetWeaponImage(UTexture2D* Texture, bool bAmmoCounterBelowGunHUD);

	void PlayGotDamageAnim();
	void PlayUseFirstAidKitAnim();
	void PlayButtonAnimation(EAnimationToPlay AnimToPlay);

	void AddElementToProgress(EUseableElement Element, ElementBar ElementProgressBar);

	void ShowWeaponOnHud(bool bShow = true);

	UFUNCTION(BlueprintImplementableEvent)
		void NewGameStartedWidgetAnimation();

#pragma region ///////////// USE FIRST AID KIT UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* UseFirstAidKidImage;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* UseFirstAidKitAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* UseFirstAidKitLessHPAnim = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HealthBar;
#pragma endregion

#pragma region ///////////// HEAL UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HealBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_HealBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_Heal;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentNumbersOfFirstAidKit;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* PressedHealButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ActiveHealAnim = nullptr;
#pragma endregion

#pragma region ///////////// DASH UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* DashBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_DashBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_Dash;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* PressedDashButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ActiveDashAnim = nullptr;
#pragma endregion

#pragma region ///////////// SLOW MOTION UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* SlowMoBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* Button_SlowMoBar;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyText_SlowMo;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* PressedSlowMotionButtonAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ActiveSlowMotionAnim = nullptr;
#pragma endregion

#pragma region ///////////// GOT DAMAGE UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* GotDamageImage;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* GotDamageAnim = nullptr;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* GotDamageLessHPAnim = nullptr;
#pragma endregion

#pragma region ///////////// WEAPON UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* WeaponImage;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* WeaponAppearAnim = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentAmmoInMagazineText;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* StoredAmmoText;
#pragma endregion

#pragma region ///////////// ITEM HOVER UI /////////////
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ItemHoverBackgroundImage;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemHoverName;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* ItemHoverDescription;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* ItemHoverImage;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ItemHoverAppearAnim = nullptr;
#pragma endregion

	// On Item Hover
	void PlayAppearAnimForItemHover(bool bForwardAnim = true);
	void SetItemHoverInformations(const FString& ItemName, const FString& ItemDescription, UTexture2D* ItemIcon);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon UI")
		FVector2D WeaponImageSizeWhenAmmoBelow = FVector2D(460.f, 260.f);
	UPROPERTY(EditDefaultsOnly, Category = "Weapon UI")
		FVector2D WeaponImageSizeWhenAmmoOnSide = FVector2D(260.f, 150.f);
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeGotDamageTime = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeFirstAidImageTime = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* PowerUpLoadedSound;

	// Progress bars (health, slow mo...)
	bool bShouldProgress;
	TMap<EUseableElement, ElementBar> WhichElementToProgress;
	void WhichElementShouldProgress(float Delta);
	void ProgressBarForUseableElements(class UProgressBar* ProgressBarElement, EUseableElement Element, UWidgetAnimation* AnimToPlayAfterFinish, float Delta);
};
