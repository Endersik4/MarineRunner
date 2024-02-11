// Copyright Adam Bartela.All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scope.generated.h"

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
	void SetUpZoomMaterial(class AGun* Gun);

	int32 Zoom(float WheelAxis, bool bShouldRestartScope = false);
	void ActiveZoom(bool bShouldActive);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USceneCaptureComponent2D* ZoomCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
		UMaterial* ZoomNotActiveMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
		UMaterial* ZoomRenderTargetMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
		TArray<float> Scope_FOVValues = { 16.f, 10.f, 5.f, 1.f };

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ZoomSound;
	
	int32 CurrentScope = 0;
	void ChangeScope(int32 CurrentScopeIndex);
	class AGun* OwningGun;

	int32 ZoomMaterialIndexOnWeapon;

	UMaterialInstanceDynamic* DynamicScopeMaterial;
};
