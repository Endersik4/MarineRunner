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

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override; //Tick

private:
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingFOV = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingChromaticAbberation = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingOffsetCA = 0.7f;

	float OriginalPlayerFOV;
	float OriginalChromaticAbberation;
	float OriginalOffsetCA;
	float FadeTimeElapsed;
	void DashEffects(float Delta);

	void PrepareDashWidget();
	class UCameraComponent* MarineCamera;
};
