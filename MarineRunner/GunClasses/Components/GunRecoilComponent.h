// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunRecoilComponent.generated.h"

UENUM(BlueprintType)
enum ERecoilType
{
	ERT_RandomValue UMETA(DisplayName = "Random Value Recoil"),
	ERT_CurveRecoil UMETA(DisplayName = "Curve Recoil"),
	ERT_None UMETA(DisplayName = "None"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UGunRecoilComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGunRecoilComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void RandomCameraRecoilTimelineProgress(float SmoothValue);

	UFUNCTION()
		void CurveCameraRecoilTimelineProgress(float NewCameraYawRotation);

	FRotator RandomBulletRotation();
	void ShootPressed();
	void ShootReleased();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		TObjectPtr<class UTimelineComponent> CameraRecoilTimeline;

	//This number will be subdivided with Recoil values
	UPROPERTY(EditAnywhere, Category = "ADS Recoil")
		float DividerOfRecoilWhileADS = 3.5f;
	//This number will be subdivided with Bullet Rotation Recoil values
	UPROPERTY(EditAnywhere, Category = "ADS Recoil")
		float DividerOfBulletRecoilWhileADS = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil")
		TSubclassOf<class UCameraShakeBase> RecoilCameraShake;
	//Use curve for Camera Recoil or use random Pitch and Yaw numbers for camera Recoil
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil")
		TEnumAsByte<ERecoilType> RecoilType;

	//Curve that is responisble for Yaw Camera Recoil (left, right)
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		TObjectPtr<UCurveFloat> YawCameraRecoilCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float FirstBulletWithoutRecoilTime = 0.3f;
	//Distance that Camera is going to rotate in pitch when shooting full magazine. 
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float PitchDistanceFromStart = 24.f;
	// Random value is picked from range and added to Recoil Camera Curve Value
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		FFloatRange RandomOffsetRecoilCurve = FFloatRange(-0.02f, 0.02f);
	// dont subtract PitchDistanceFromStart from Target Player Camera Rotation if Yaw distance between initial player rotation and current is greater then MaxYawDistanceToSubstract
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float MaxYawDistanceToSubstract = 80.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float MaxDistanceForCameraToGoBack = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float BackToInitialCameraRotationSpeed = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_CurveRecoil", EditConditionHides))
		float MouseCancelCameraTolerance = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_RandomValue", EditConditionHides))
		FFloatRange RandomCameraRecoilPitchRange = FFloatRange(20, 10);
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_RandomValue", EditConditionHides))
		FFloatRange RandomCameraRecoilYawRange = FFloatRange(18, 10);
	//The curve responsible for how quickly the screen reaches RandomCameraRecoilPitch and RandomCameraRecoilYaw and returns to its rotation. 
	//It was added for smoothness. Curve Value is multiply by Camera Recoils values, to add smoothness use 0-1 values 
	UPROPERTY(EditDefaultsOnly, Category = "Camera Recoil", meta = (EditCondition = "RecoilType == ERecoilType::ERT_RandomValue", EditConditionHides))
		TObjectPtr<UCurveFloat> RandomCameraRecoil_SmoothCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet Recoil")
		FFloatRange RandomBulletRecoilPitch = FFloatRange(-5, 5);
	UPROPERTY(EditDefaultsOnly, Category = "Bullet Recoil")
		FFloatRange RandomBulletRecoilYaw = FFloatRange(-5, 5);

	void SetTimelineAccordingToRecoilType();
	void PlayRecoil();

	bool bCanStartCameraRecoil = true;

	bool bRotateCameraToPitchDistance;
	// move the camera to PitchDistanceFromStart in time. The time is calculated by multiplying the original magazine capacity by the shooting time.
	void RotatePlayerCameraToPitchDistance(float Delta);

	float RandomYawOffset_CurveRecoil;
	FRotator TargetPlayerCameraRotation;
	bool bRotatePlayerCameraToTargetRotation;
	void BackPlayerCameraToInitialRotation();
	void CameraInterpBackToInitialPosition(float Delta);

	float RandomCameraRecoilYaw;
	float RandomCameraRecoilPitch;

	void ResetVariablesForCameraRecoil();

	bool bFirstBulletWithoutRecoil = true;
	FTimerHandle FirstBulletHandle;
	void ShouldFirstBulletGoStraight() { bFirstBulletWithoutRecoil = true; }

	UPROPERTY(Transient)
		TObjectPtr<class AGun> Gun;
	UPROPERTY(Transient)
		TObjectPtr<class AMarinePlayerController> PlayerController;

};
