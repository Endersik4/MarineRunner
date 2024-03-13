// Copyright Adam Bartela.All Rights Reserved


#include "SelectFloorEntryObject.h"

bool USelectFloorEntryObject::operator==(const USelectFloorEntryObject& OtherElevatorPanel)
{
    return ElevatorFloor.Floor == OtherElevatorPanel.ElevatorFloor.Floor;
}

bool USelectFloorEntryObject::operator==(const int32 FloorToGo)
{
    return ElevatorFloor.Floor == FloorToGo;

}
bool USelectFloorEntryObject::operator!=(const int32 FloorToGo)
{
    return ElevatorFloor.Floor != FloorToGo;

}
