// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

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
	//if bSetStoredAmmo == false then set ammo text in CurrentAmmoInMagazineText
	//otherwise set ammo in StoredAmmoText
	void SetAmmoText(int32 Ammo, bool bSetStoredAmmo = false);
	void SetWeaponImage(UTexture2D* Texture);
	void SetGotDamage(bool bGot);

	void HideWeaponThings(bool bShouldHide);


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* WeaponImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* CurrentAmmoInMagazineText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* StoredAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage* GotDamageImage;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeTime = 1.5f;

	bool bGotDamage;
	float FadeTimeElapsed;
	void FadeGotDamageImage(float Delta);

	void SetUpMarinePawn();
	class AMarineCharacter* MarinePawn;
};
