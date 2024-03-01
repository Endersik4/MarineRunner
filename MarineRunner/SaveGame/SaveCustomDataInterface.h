	// Copyright Adam Bartela.All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveCustomDataInterface.generated.h"

UENUM(BlueprintType)
enum ESavedDataState
{
	ESDS_SpawnObject,
	ESDS_LoadData
};

USTRUCT(BlueprintType)
struct FCustomDataSaved
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
		TEnumAsByte<ESavedDataState> SavedDataState;

	UPROPERTY(EditAnywhere)
		AActor* ObjectToSaveData;
	UPROPERTY(EditAnywhere)
		int32 ObjectState;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AActor> ObjectToSpawnFromClass;
	UPROPERTY(EditAnywhere)
		FTransform ObjectTransform;
	UPROPERTY(EditAnywhere)
		float ValueToSave;
	UPROPERTY(EditAnywhere)
		bool bValueNotSavedWhileInGame;

	FCustomDataSaved()
	{
		SavedDataState = ESavedDataState::ESDS_LoadData;
		ObjectToSaveData = nullptr;
		ObjectState = 0;
		ObjectToSpawnFromClass = nullptr;
		ObjectTransform = FTransform();
		ValueToSave = 0.f;
		bValueNotSavedWhileInGame = false;
	}


	FCustomDataSaved(const TEnumAsByte<ESavedDataState>& _SavedDataState, AActor* _ObjectToSaveData,
		const int32& _ObjectState, const float& _ValueToSave = 0, const bool& _ValueNotSavedWhileInGame = false)
	{
		SavedDataState = _SavedDataState;
		ObjectToSaveData = _ObjectToSaveData;
		ObjectState = _ObjectState;
		ValueToSave = _ValueToSave;
		bValueNotSavedWhileInGame = _ValueNotSavedWhileInGame;
	}

	FCustomDataSaved(const TEnumAsByte<ESavedDataState>& _SavedDataState, TSubclassOf<AActor> _ObjectToSpawnFromClass,
		const FTransform& _ObjectTransform, const float& _ValueToSave = 0, const bool& _ValueNotSavedWhileInGame = false)
	{
		SavedDataState = _SavedDataState;
		ObjectToSpawnFromClass = _ObjectToSpawnFromClass;
		ObjectTransform = _ObjectTransform;
		ValueToSave = _ValueToSave;
		bValueNotSavedWhileInGame = _ValueNotSavedWhileInGame;

	}

	FCustomDataSaved(const TEnumAsByte<ESavedDataState>& _SavedDataState,
		AActor* _ObjectToSaveData,
		const int32& _ObjectState, TSubclassOf<AActor> _ObjectToSpawnFromClass,
		const FTransform& _ObjectTransform, const float& _ValueToSave = 0, const bool& _ValueNotSavedWhileInGame = false)
	{
		SavedDataState = _SavedDataState;
		ObjectToSaveData = _ObjectToSaveData;
		ObjectState = _ObjectState;
		ObjectToSpawnFromClass = _ObjectToSpawnFromClass;
		ObjectTransform = _ObjectTransform;
		ValueToSave = _ValueToSave;
		bValueNotSavedWhileInGame = _ValueNotSavedWhileInGame;
	}

	FCustomDataSaved& operator=(const FCustomDataSaved& OtherObject)
	{
		SavedDataState = OtherObject.SavedDataState;
		ObjectToSaveData = OtherObject.ObjectToSaveData;
		ObjectState = OtherObject.ObjectState;
		ObjectToSpawnFromClass = OtherObject.ObjectToSpawnFromClass;
		ObjectTransform = OtherObject.ObjectTransform;
		ValueToSave = OtherObject.ValueToSave;
		bValueNotSavedWhileInGame = OtherObject.bValueNotSavedWhileInGame;
		return *this;
	}
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USaveCustomDataInterface : public UInterface
{
	GENERATED_BODY()
};

class MARINERUNNER_API ISaveCustomDataInterface
{
	GENERATED_BODY()

public:

	UFUNCTION()
		virtual void LoadData(const int32 IDkey, const FCustomDataSaved& SavedCustomData) = 0;

	UFUNCTION()
		virtual void SaveData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) = 0;

	UFUNCTION()
		virtual void RestartData(class ASavedDataObject* SavedDataObject, const int32 IDkey, const FCustomDataSaved& SavedCustomData) = 0;
};