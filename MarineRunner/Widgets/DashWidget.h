// Copyright Adam Bartela.All Rights Reserved

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
		TObjectPtr<class UImage> DashImage;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float FadeTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingFOV = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingChromaticAbberation = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Fading Image")
		float StartingOffsetCA = 0.7f;

	bool bRemoveDashWidget;
	void PrepareDashWidget();

	float OriginalPlayerFOV;
	float OriginalChromaticAbberation;
	float OriginalOffsetCA;
	float FadeTimeElapsed;
	void DashEffects(const float & Delta);

	UPROPERTY(Transient)
		TObjectPtr<class AMarineCharacter> MarinePawn;
	UPROPERTY(Transient)
		TObjectPtr<class UCameraComponent> PlayerCamera;
};
