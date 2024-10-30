#include <string>
#include <stdlib.h>
#include "Tile.h"
#include "GameBuilding.h"
using namespace std;

void Tile::placeBuildingOrStreet(int pos, GameBuilding building)
{
    if (building.buildingType == Settlement || building.buildingType == City)
    {
        buildingPlacement[pos] = building;
    }
    else
    {
        streetPlacement[pos] = building;
    }
}

string Tile::toString()
{

    string str = "TileObject( ";
    switch (type)
    {
    case hills:
        str.append("TileType: Hills ");
        break;
    case forest:
        str.append("TileType: Forest ");
        break;
    case mountains:
        str.append("TileType: Mountains ");
        break;
    case fields:
        str.append("TileType: Fields ");
        break;
    case pastures:
        str.append("TileType: Pastures ");
        break;
    case desert:
        str.append("TileType: Desert ");
        break;
    case invisible:
        str.append("TileType: Invisible ");
        break;
    default:
        break;
    }
    str.append("MarkerID: " + to_string(MarkerID) + " ");
    str.append("DiceNumber: " + to_string(diceNumber) + " ");
    //str.append("buildingPlacement: " + buildingPlacement);
    //str.append("streetPlacement: " + streetPlacement);
    str.append(")");
    return str;
}