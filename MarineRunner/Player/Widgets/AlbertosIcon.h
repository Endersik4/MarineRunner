// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AlbertosIcon.generated.h"

/**
 * 
 */
UCLASS()
class MARINERUNNER_API UAlbertosIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetAlbertosPawn(TObjectPtr<class AAlbertosPawn> NewAlbertosPawn) { AlbertosPawn = NewAlbertosPawn; }

	void StartTrackingAlbertos();
	void StopTrackingAlbertos();

protected:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> AlbertosIconCanvasPanel = nullptr;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<class UImage> AlbertosIconImage = nullptr;
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Icon Settings")
	float TrackAlbertosLocationTime = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Albertos Icon Settings")
	FName IconLocationSocketName = FName(TEXT("HeadSocket"));

	UPROPERTY(Transient)
	FTimerHandle TrackAlbertosLocationHandle = FTimerHandle();
	UPROPERTY(Transient)
	FVector2D ViewportSize = FVector2D(0.f);
	void UpdateAlbertosIconPosition();

	UPROPERTY(Transient)
	TObjectPtr<class UCanvasPanelSlot> AlbertosIconPanelSlot = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<class AAlbertosPawn> AlbertosPawn = nullptr;
};
