// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Framework/MarineRunnerGameInstance.h"

void UMarineRunnerGameInstance::Init()
{
	Super::Init();
	SetSaveNumberAccordingToNumOfFiles();
}

void UMarineRunnerGameInstance::SetSaveNumberAccordingToNumOfFiles()
{
	FString  WildCard = "*ManualSave*";
	const int32 NumberOfFilesInOneSave = 3; //.json, .png, .sav
	CurrentSaveNumber = GetAllFilesWithName(WildCard) / NumberOfFilesInOneSave;
}
