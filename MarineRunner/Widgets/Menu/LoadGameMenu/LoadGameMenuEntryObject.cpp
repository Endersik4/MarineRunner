// Fill out your copyright notice in the Description page of Project Settings.


#include "MarineRunner/Widgets/Menu/LoadGameMenu/LoadGameMenuEntryObject.h"

bool ULoadGameMenuEntryObject::operator<(const ULoadGameMenuEntryObject& OtherObject)
{
    UE_LOG(LogTemp, Warning, TEXT("POROWNANIE %d z %d a wyszlo %s"), SavesMenuData.SaveNumber, OtherObject.SavesMenuData.SaveNumber, SavesMenuData.SaveNumber < OtherObject.SavesMenuData.SaveNumber
        ? TEXT("TRUE") : TEXT("FALSe"));
    return SavesMenuData.SaveNumber < OtherObject.SavesMenuData.SaveNumber;
}
