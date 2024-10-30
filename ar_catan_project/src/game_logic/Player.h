#pragma once

#include <array>
#include "Tile.h"

class Player
{
public:
    int points; // number of points the player has
    int playerID;
    glm::vec4 color;

    std::array<int, 5> resources; // array of resources, order -> brick, lumber, ore, grain, wool

    Player(int pID, glm::vec4 in_color) : points(0)
    {
        playerID = pID;
        color = in_color;
    }

    void addResource(RessourceType resourceType, int amount);

    void halveResources();

    void addPoints(int amount);

    bool payResourcesForRoad();
    bool payResourcesForSettlement();
    bool payResourcesForCity();
};