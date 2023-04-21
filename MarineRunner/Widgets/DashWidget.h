// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashWidget.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UDashWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
		class UImage* DashImage;

	void SetFadeTime(float NewTime) { FadeTime = NewTime; }
	void ShouldAddChangingFov(bool bWant) { bShouldAddFov = bWant; }

protected:
	virtual void NativeConstruct() override; //konstruktor
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; //Tick
	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget)) //Dodawanie zmiennych
	//	class UImage* DashImage; //Aby to zadzialalo w widget blueprint trzeba 
	//Stworzyc UUserWidget na podstawie TEJ Klasy i pozniej dodac Image i 
	//nazwac tak samo jak tutaj a pozniej prawy klawisz myszki wrap->Canvas Panel

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

	bool bShouldAddFov;
	float Fov;
	float FadeTimeElapsed;
	void FadingImage(float Delta);

	void SetMarinePawn();	
	class UCameraComponent* MarineCamera;
};
