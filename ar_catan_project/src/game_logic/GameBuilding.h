#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <random>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

typedef enum BuildingType
{
    Road,
    Settlement,
    City,
    DefaultBuilding
};

class GameBuilding
{
public:
    GameBuilding();
    GameBuilding(int in_playerID, glm::mat4 in_relativeToWorldReference, BuildingType in_buildingType, int in_buildingID);
    GameBuilding(const GameBuilding &other);
    int playerID;
    int buildingID;
    //Mat PmatrixOnSpawn;
    //GridSystem gridSystem;
    glm::mat4 relativeToWorldReference;
    BuildingType buildingType;
    //int gridID;
};