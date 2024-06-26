// Copyright Adam Bartela.All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScopeGunMod.generated.h"

// Scope for The Gun by changing FOV in another camera. 
// Zoom can be changed by mouse wheel
UCLASS()
class MARINERUNNER_API AScope : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AScope();

	FORCEINLINE void SetZoomMaterialIndexOnWeapon(int32 NewIndex) { ZoomMaterialIndexOnWeapon = NewIndex; }
	void SetUpZoomMaterial(TObjectPtr<class AGun> Gun);

	const int32 Zoom(const float WheelAxis, const bool bShouldRestartScope = false);
	void ActiveZoom(bool bShouldActive);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USceneCaptureComponent2D> ZoomCamera = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
	TObjectPtr<UMaterial> ZoomNotActiveMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
	TObjectPtr<UMaterial> ZoomRenderTargetMaterial = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
	TArray<float> Scope_FOVValues = { 16.f, 10.f, 5.f, 1.f };
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
	TObjectPtr<USoundBase> ZoomSound = nullptr;

	UPROPERTY(Transient)
	int32 CurrentScope = 0;
	void ChangeScope(int32 CurrentScopeIndex);

	UPROPERTY(Transient)
	TObjectPtr<class AGun> OwningGun = nullptr;
	UPROPERTY(Transient)
	int32 ZoomMaterialIndexOnWeapon = 0;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> DynamicScopeMaterial = nullptr;
};
