// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveGameJsonFile.generated.h"


class FJsonObject;

UCLASS()
class MARINERUNNER_API USaveGameJsonFile : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static bool ReadJson(const FString& JsonFilePath, TSharedPtr<FJsonObject>& JsonObject);
	
	static void WriteJson(const TSharedPtr<FJsonObject>& JsonObject,const FString & JsonFilePath);

};
