// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scope.generated.h"

USTRUCT(BlueprintType)
struct FZoomStruct {
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere, Category = "Zoom")
		float FOVValue;
	UPROPERTY(EditAnywhere, Category = "Zoom")
		float ScopeCrosshairScale;

	FZoomStruct()
	{
		FOVValue = 0.f;
		ScopeCrosshairScale = 0.f;
	}

	FZoomStruct(float NewFOVValue, float NewCrosshairScale)
	{
		FOVValue = NewFOVValue;
		ScopeCrosshairScale = NewCrosshairScale;
	}
};

// Scope for The Gun by changing FOV in another camera. 
// Zoom can be changed by mouse wheel
UCLASS()
class MARINERUNNER_API AScope : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScope();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		int32 Zoom(float WheelAxis, bool bShouldRestartScope = false);

	UFUNCTION(BlueprintCallable, meta = (AllowPrivateAccess = "true"))
		void ActiveZoom(bool bShouldActive);

	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USceneComponent* SceneRootComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class USceneCaptureComponent2D* ZoomCamera;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Scope_Mesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UStaticMeshComponent* Scope_Crosshair;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class URectLightComponent* RectLightForScope;

	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInterface* ZoomMaterial;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
		UMaterialInterface* RenderTargetMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ZoomSound;

	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
		TArray<FZoomStruct> ZoomData = {FZoomStruct(16.f, 0.053f), FZoomStruct(10.f, 0.034f), FZoomStruct(5.f, 0.017f), FZoomStruct(1.f, 0.0034f)};

	int32 CurrentScope = 0;
	void ChangeScope(int32 CurrentScopeIndex);
};
