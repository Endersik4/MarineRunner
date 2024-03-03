// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArmsSwayComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UArmsSwayComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UArmsSwayComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE void SetWeaponSwayDivider(float NewSwayDivder) { WeaponSwayDivider = NewSwayDivder; }
	FORCEINLINE void SetWeaponSwayWhileMovingDivider(float NewSwayWhileMovingDivder) { WeaponSwayWhileMovingDivider = NewSwayWhileMovingDivder; }
	FORCEINLINE void SetInADS(bool bNewIsnADS) { bInADS = bNewIsnADS; }

	void ResetArmsLocation();

private:
	//Rotation Sway Pitch
//Maximal Pitch rotation of Gun when player looking down
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeBack = -2.f;
	//Maximal Pitch rotation of Gun when player looking up
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayPitchRangeUp = 2.f;
	//Speed of Gun that will reach the Range Sway Pitch
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayPitch = 7.f;

	//Rotation Sway Yaw
	//Maximal Yaw rotation of Gun when player looking left
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeBack = -2.75f;
	//Maximal Yaw rotation of Gun when player looking right
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float RotationSwayYawRangeUp = 2.75f;
	//Speed of Gun that will reach the Range Sway Yaw
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway")
		float SpeedOfSwayYaw = 3.f;

	//Location Sway X
//Maximal X location of Gun when player moving forward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeBack = -3.f;
	//Maximal X location of Gun when player moving backward (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayXRangeUp = 3.f;
	//Speed of Gun that will reach the Range Sway X
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayX = 7.f;

	//Location Sway Y
	//Maximal Y location of Gun when player moving right (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeBack = -2.f;
	//Maximal Y location of Gun when player moving left (This value is added to RelativeLocationInPawn)
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float LocationSwayYRangeUp = 2.f;
	//Speed of Gun that will reach the Range Sway Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayY = 4.f;

	//Speed of Moving Gun while Player is moving
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float SpeedOfSwayWhileMoving = 550.f;
	//Multiplier of Calculeted Y by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Y
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationYSwayWhileMoving = 0.8f;
	//Multiplier of Calculeted Z by Lemniscate Of Bernoulli, this represents how big Gun is gonna move in Z
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Setting Up Gun Sway While Moving")
		float MultiplierOfLocationZSwayWhileMoving = 0.4f;

	float WeaponSwayDivider = 1.f;
	float WeaponSwayWhileMovingDivider = 1.f;
	bool bInADS = false;

	void CalculateGunSway(FVector& CalculatedLocation, FRotator& CalculatedRotation, float Delta);
	void GunSway(float Delta);
	FRotator GunRotationSway;
	FVector RelativeLocationInPawn;

	FVector CalculateLOBGunSwayWhileMoving();
	void GunSwayWhileMoving();

	class AMarineCharacter* Player;
	APlayerController* PlayerController;
};
