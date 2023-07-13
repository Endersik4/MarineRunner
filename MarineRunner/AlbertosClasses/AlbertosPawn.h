// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AlbertosPawn.generated.h"

UCLASS()
class MARINERUNNER_API AAlbertosPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAlbertosPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UUserWidget* GetCraftingTableWidget() const;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UCapsuleComponent* AlbertosCapsuleComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USkeletalMeshComponent* AlbertosSkeletalMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UWidgetComponent* CraftingTableWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		class UFloatingPawnMovement* AlbertosFloatingMovement;

};
