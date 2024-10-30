#include "GameBuilding.h"

GameBuilding::GameBuilding()
{
}

GameBuilding::GameBuilding(int in_playerID, glm::mat4 in_relativeToWorldReference, BuildingType in_buildingType, int in_buildingID)
{
    playerID = in_playerID;
    relativeToWorldReference = in_relativeToWorldReference;
    buildingType = in_buildingType;
    buildingID = in_buildingID;
}

GameBuilding::GameBuilding(const GameBuilding &other)
    : playerID(other.playerID), relativeToWorldReference(other.relativeToWorldReference), buildingType(other.buildingType),buildingID(other.buildingID)
{
}
