// Copyright Adam Bartela.All Rights Reserved


#include "JsonFileActions.h"

bool USaveGameJsonFile::ReadJson(const FString& JsonFilePath, TSharedPtr<FJsonObject>& JsonObject)
{
	FString JsonString;
	FFileHelper::LoadFileToString(JsonString, *JsonFilePath);

	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		return true;
	}
	else 
		return false;
}

void USaveGameJsonFile::WriteJson(const TSharedPtr<FJsonObject>& JsonObject, const FString& JsonFilePath)
{
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FFileHelper::SaveStringToFile(OutputString, *JsonFilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get());
}
