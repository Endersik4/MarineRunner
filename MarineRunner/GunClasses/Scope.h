// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scope.generated.h"

//Scope for Gun by changing FOV in another camera. 
//Zoom can be changed by mouse wheel
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

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ZoomSound;


	// Keys are FOV, the more FOV, the higher the magnification of the scope.
	// Values are the Scale of the viewfinder displayed on the scope during magnification.
	UPROPERTY(EditDefaultsOnly, Category = "Scope Settings")
		TMap<float, float> FOVZoom = {
			{16.f, 0.053f},
			{10.f, 0.034f},
			{5.f, 0.017f},
			{1.f, 0.0034f}
		};

	int32 CurrentScope = 0;
	TArray<float> FOVZoom_Keys;
	void SetNewScope(int32 CurrentScopeIndex);
};
