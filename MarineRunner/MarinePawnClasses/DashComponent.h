// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MARINERUNNER_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MakeDashWidget(bool bShouldMake, float FadeTime, bool bAddFov = true, bool bAddChromaticAbberation = true);

	void Dash();
	void RemoveDashWidget();
	
	bool GetIsPerformingDash() const { return bIsPerformingDash; }
private:
	//Dash Speed
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashForce = 6000.f;
	//Dash cooldown in Seconds
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashCoolDown = 1.4f;
	//How long Player Dash in Seconds (DashForce is applied per frame so after DashLength it will stop apply dash froce to the player)
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashLength = 0.1f;
	//How long DashWidget will be in player viewport
	UPROPERTY(EditAnywhere, Category = "Dash Settings")
		float DashWidgetTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Settings")
		TSubclassOf<class UUserWidget> DashWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dash Sounds")
		USoundBase* DashSound;


	bool bCanDash = true;
	bool bIsPerformingDash;
	float OriginalForce;

	void DashLengthTimer();
	void DashOnRamp();

	FORCEINLINE void EndDashCooldown() { bCanDash = true; }

	FTimerHandle DashLengthHandle;
	FTimerHandle DashCooldownHandle;


	bool CanPlayerPerformDash() const;

	class UDashWidget* DashWidget;
	class AMarineCharacter* MarinePawn;

};
