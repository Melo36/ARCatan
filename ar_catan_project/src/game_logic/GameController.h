#pragma once

/*********
* GameController.h
*********/

#ifndef GameController_H
#define GameController_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/string_cast.hpp>

#include <map>
#include <math.h>
#include "Player.h"
#include <string>
#include <map>
#include "Tile.h"
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <random>
#include "GameBuilding.h"
#include "../CameraController.h"

using namespace glm;
using namespace std;

template <class C, typename T>
bool contains(C &&c, T e)
{
    return std::find(begin(c), end(c), e) != end(c);
}

class GameController
{
    //MarkerID-Setup: desertMarker, allTerrainMarkers, placeBuildingMarkers, and nextTurnMarker
public:
    int desertMarker = 0x599d ; // used as world-reference (expandable...)
    mat4 desertMarkerPose = mat4(1.0);
    int desertMarkerLostFrames = 0;
    // DiceNumbers and TerrainMarkers are sorted Arrays that correspond to each other; TODO a variable diceNumber for each Tile would be better
    int allTerrainMarkers[18] =
    { 0x227d, 0x21ee, 0x0125,
    0x0fab, 0x451e, 0x1d59, 0x332a,
 0x750b, 0x506b,       0x128f, 0x30a9,
    0x72db, 0x3b0d, 0x12e4, 0x0a91,
        0x2bf2,0x347a , 0x2d57 };
                
        /*
        { 0x347a, 0x72db, 0x750b, 
      0x3b0d, 0x2bf2, 0x506b, 0x0fab,
  0x2d57, 0x12e4,       0x451e, 0x227d,
      0x0a91, 0x128f, 0x1d59, 0x21ee, 
          0x30a9, 0x332a, 0x0125};*/ //sorted as Map Numbering which can be seen in the Method GameController.cpp::assignNeighbors()
    int allDiceNumbers[18] = {2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12};

    Tile terrain[19] = {
        Tile(hills), Tile(hills), Tile(hills),
        Tile(forest), Tile(forest), Tile(forest), Tile(forest),
        Tile(mountains), Tile(mountains), Tile(desert), Tile(mountains), Tile(fields),
        Tile(fields), Tile(fields), Tile(fields), Tile(pastures),
        Tile(pastures), Tile(pastures), Tile(pastures)}; // expandable with the harbor trading feature

    //Players
    Player allPlayers[2] = {Player(0,glm::vec4(1.0,0.0,0.0,1.0)), Player(1,glm::vec4(0.0,1.0,0.0,1.0))};
    int currentPlayerID;
    int nextTurnMarker = 0x99ff;
    int lastDiceRoll;

    //AllBuilding data
    vector<GameBuilding> allBuildings;
    map<int, int> newDetectedMarkers;                                                                                                // <markerID, frames since detection>
    map<int, BuildingType> placeBuildingMarkers = {make_pair(0x3ec3, Road), make_pair(0x066e, Settlement), make_pair(0x1a77, City)}; // road, settlement, city

    //init game functions
    void initializeGame();
    void shuffleTerrain();
    void shuffleDiceNumbers();
    void assignNeighbors();

    //detect placing building with markers
    mat4 getWorldReferenceMarker(std::vector<int> &MarkerID, std::vector<mat4> &MarkerPmat);
    void updateNewDetectedMarkers(vector<int> MarkerID);
    void placeBuildingsOnLongLastingMarkers(std::vector<int> markerIDs, std::vector<mat4> pMat);
    pair<pair<int, int>, glm::vec3> getTileAndPositionForNewBuilding(mat4 relativeToWorldReference, BuildingType newBuildingType);

    //game logic for building placement
    bool checkCorrectBuildingPlacement(GameBuilding newBuilding, pair<int, int> tileAndPos);
    void placeBuildingOrStreetonMap(int tile, int pos, GameBuilding building);
    void placeBuildingOrStreet(int tile, int pos, GameBuilding building);

    void halvePlayerRessources();
    void placeSettlementAndStreetAtStartOfGame(int playerID, mat4 pos);
    void collectPlayerRessources(int dice);
    int gameOver();
    void nextTurn();
    //void startGame();
    Player getActivePlayer();

    int buildingID;
};
#endif
