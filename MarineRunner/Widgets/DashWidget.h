// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashWidget.generated.h"

/**
 *  Dash Effects
 */
UCLASS()
class MARINERUNNER_API UDashWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* DashImage;

	void SetFadeTime(float NewTime) { FadeTime = NewTime; }
	void ShouldAddChromaticAbberation(bool bShould) { bShouldAddChromaticAbberation = bShould; }
	void ShouldAddChangingFov(bool bShould) { bShouldAddFov = bShould; }

protected:
	virtual void NativeConstruct() override; //konstruktor
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; //Tick

private:
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingFOV = 10.f;
	//Chromatic Abberation
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingChromaticAbberation = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingOffsetCA = 0.7f;

	bool bShouldAddFov = true;
	bool bShouldAddChromaticAbberation = true;
	float Fov;
	float FadeTimeElapsed;
	void FadingImage();

	void SetMarinePawn();	
	class UCameraComponent* MarineCamera;
};
