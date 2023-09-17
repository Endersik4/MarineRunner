// Fill out your copyright notice in the Description page of Project Settings.


#include "MarinePlayerController.h"
static TAutoConsoleVariable<int32> MouseSensitivity(
	// Name
	TEXT("MouseSensitivity"),

	// Default value
	0.7f,

	// Documentation
	TEXT("Value responsible for Mouse Sensitivity.\n"),

	// Flags
	ECVF_SetByConsole | ECVF_RenderThreadSafe);

void MouseSensitivityCallback(IConsoleVariable* Var)
{
	UE_LOG(LogTemp, Warning, TEXT("working"));
}

void AMarinePlayerController::BeginPlay()
{
	Super::BeginPlay();

	MouseSensitivity.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&MouseSensitivityCallback));
}