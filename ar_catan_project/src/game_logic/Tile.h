#pragma once

#include <string>
#include <map>
#include "GameBuilding.h"
using namespace std;

typedef enum TileType
{
    hills,
    forest,
    mountains,
    fields,
    pastures,
    desert,
    invisible
};

typedef enum RessourceType
{
    brick,
    lumber,
    ore,
    grain,
    wool,
    nothing
};

//map<TileType, RessourceType> TileRessouceMapping = { make_pair(hills, brick) };

class Tile
{

public:
    void placeBuildingOrStreet(int pos, GameBuilding building);
    map<int, GameBuilding> buildingPlacement;
    map<int, GameBuilding> streetPlacement;
    Tile *rightNeighbor;
    Tile *leftNeighbor;
    Tile *topRightNeighbor;
    Tile *topLeftNeighbor;
    Tile *bottomRightNeighbor;
    Tile *bottomLeftNeighbor;

    TileType type;
    int diceNumber;
    int MarkerID;

    Tile(TileType typ)
    {
        type = typ;
    }

    Tile()
    {
        type = invisible;
    }

    string toString();
};
