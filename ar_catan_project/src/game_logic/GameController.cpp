#include "GameController.h"
#include "GameBuilding.h"
#include <glm/gtc/type_ptr.hpp>


int main3()
{
    int dice = rand() % 12 + 2;
    std::cout << dice + "was rolled";
    if (dice == 7)
    {
    }

    return 0;
}

void GameController::initializeGame()
{
    currentPlayerID = 0;
    buildingID = 0;

    shuffleDiceNumbers();
    shuffleTerrain();
    assignNeighbors();

    //assign each Tile a MarkerID and a DiceNumber
    for (int i = 0; i < 18; i++)
    {
        if (i >= 9)
        {
            terrain[i + 1].MarkerID = allTerrainMarkers[i];
            terrain[i + 1].diceNumber = allDiceNumbers[i];
        }
        else
        {
            terrain[i].MarkerID = allTerrainMarkers[i];
            terrain[i].diceNumber = allDiceNumbers[i];
        }
        terrain[9].MarkerID = desertMarker;
        cout << terrain[i].toString() << endl;
    }

    /*
    std::vector<string> mapTileTypeToString = { "hills","forest","mountains","fields","pastures","desert","invisible" };

    for (int i = 0; i < 19; i++) {
        cout << "Tile " << i << endl;
        if (terrain[i].bottomLeftNeighbor != nullptr) {
            cout << "BL " << mapTileTypeToString[terrain[i].bottomLeftNeighbor->type] << terrain[i].bottomLeftNeighbor->diceNumber << endl;
        }
        if (terrain[i].bottomRightNeighbor != nullptr) {
            cout << "BR " << mapTileTypeToString[terrain[i].bottomRightNeighbor->type] << terrain[i].bottomRightNeighbor->diceNumber << endl;
        }
        if (terrain[i].leftNeighbor != nullptr) {
            cout << "L " << mapTileTypeToString[terrain[i].leftNeighbor->type] << terrain[i].leftNeighbor->diceNumber << endl;
        }
        if (terrain[i].rightNeighbor != nullptr) {
            cout << "R " << mapTileTypeToString[terrain[i].rightNeighbor->type] << terrain[i].rightNeighbor->diceNumber << endl;
        }
        if (terrain[i].topLeftNeighbor != nullptr) {
            cout << "TL " << mapTileTypeToString[terrain[i].topLeftNeighbor->type] << terrain[i].topLeftNeighbor->diceNumber << endl;
        }
        if (terrain[i].topRightNeighbor != nullptr) {
            cout << "TR " << mapTileTypeToString[terrain[i].topRightNeighbor->type] << terrain[i].topRightNeighbor->diceNumber << endl;
        }
    }*/

    //assign starting Ressources
    int sA = 2;
    for (int i = 0; i < 2; i++) {
        allPlayers[i].addResource(brick, sA);
        allPlayers[i].addResource(lumber, sA);
        allPlayers[i].addResource(grain, sA);
        allPlayers[i].addResource(ore, sA);
        allPlayers[i].addResource(wool, sA);
    }
}

void GameController::shuffleTerrain()
{
    std::shuffle(terrain, terrain + 19, std::default_random_engine(0));

    //put desert back in the middle
    for (int i = 0; i < 19; i++)
    {
        if (terrain[i].type == desert)
        {
            Tile temp = terrain[9];
            terrain[9] = terrain[i];
            terrain[i] = temp;
        }
    }
}

void GameController::shuffleDiceNumbers()
{
    std::shuffle(allDiceNumbers, allDiceNumbers + 17, std::default_random_engine(0));
}
/*
void GameController::setupTerrainMarkers()
{
    // TODO distribute (randomly or always the same ???) allTerrainMarkers into the 5 Terrain-Marker-arrays
}*/

void GameController::nextTurn()
{
    currentPlayerID = currentPlayerID == 0 ? 1 : 0;
    int winner = gameOver();
    if (winner == 0)
    {
        std::cout << "Player 1 wins";
        return;
    }
    else if (winner == 1)
    {
        std::cout << "Player 2 wins";
        return;
    }

    //gewürfelt am Anfang jeder Runde
    lastDiceRoll = rand() % 11 + 2;
    std::cout << lastDiceRoll + "was rolled";
    if (lastDiceRoll == 7)
    {
        halvePlayerRessources();
    }
    else
    {
        collectPlayerRessources(lastDiceRoll);
    }

    //Spieler kann ein Gebäude platzieren und dann den Zug beenden
}

// am Anfang kann jeder abwechselnd Gebäude platzieren
//void GameController::startGame()
//{
//    // TODO: get this Matrix from player interaction and convert it to tile and pos
//    //Player 1 hat ID 0 und Player 2 hat ID 1
//    int arr[16];
//    mat4 temp = make_mat4(arr);
//    placeSettlementAndStreetAtStartOfGame(0, temp);
//    placeSettlementAndStreetAtStartOfGame(1, temp);
//    placeSettlementAndStreetAtStartOfGame(0, temp);
//    placeSettlementAndStreetAtStartOfGame(1, temp);
//
//    // jeder Spieler bekommt Resourcen
//    collectPlayerRessources(-1);
//}

Player GameController::getActivePlayer()
{

    return allPlayers[currentPlayerID];
}

void GameController::updateNewDetectedMarkers(vector<int> MarkerID)
{

    for (int i = 0; i < MarkerID.size(); i++)
    {

        int curMarkerID = MarkerID[i];
        //ignore a GameBoardMarker
        if (contains(allTerrainMarkers, MarkerID[i]) || desertMarker == MarkerID[i])
            continue;

        if (placeBuildingMarkers.count(MarkerID[i]) == 0 && MarkerID[i] != nextTurnMarker)
        {
            //cout << "no registered marker to place Buildings" << endl;
            continue;
        }

        //new Maker detected
        if (newDetectedMarkers.count(MarkerID[i]) == 0)
        {
            //cout << "added new marker ";
            newDetectedMarkers.insert(make_pair(MarkerID[i], 1));
        }
        //Marker update; increase framecount by one
        if (newDetectedMarkers.count(MarkerID[i]))
        {
            //cout << "updated frame count ";
            int newFrameValue = newDetectedMarkers.find(MarkerID[i])->second;
            newFrameValue++;
            //cout << to_string(newFrameValue) + " ";
            newDetectedMarkers.insert_or_assign(MarkerID[i], newFrameValue);
        }
    }
    //not detected anymore; remove it
    for (map<int, int>::iterator it = newDetectedMarkers.begin(); it != newDetectedMarkers.end() /* not hoisted */; /* no inc. */)
    {
        if (!contains(MarkerID, it->first))
        {
            //cout << "remove marker " << it->first;
            it = newDetectedMarkers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

glm::vec3 getNewBuildingLocation(mat4 relativeToWorldReference)
{
    glm::vec3 relBuildingTrans = glm::vec3(relativeToWorldReference[3]);
    glm::vec3 absBuildingTrans = relBuildingTrans;
    glm::vec3 newBuildingPlacement;
    //Ray
    glm::vec3 Ray_ori = glm::inverse(CameraController::getInstance()->getView())[3]; //CameraPosition
    glm::vec3 Ray_dir = glm::normalize(absBuildingTrans - Ray_ori);
    float t;
    //Plane
    glm::vec3 Plane_norm = glm::vec3(0, 1, 0);
    glm::vec3 Plane_ori = glm::vec3(0, 0.0, 0); //testing height ob buildings above tiles

    // assuming vectors are all normalized
    float denom = glm::dot(Plane_norm, Ray_dir);
    //if (denom > 1e-6) {
    glm::vec3 p0l0 = Plane_ori - Ray_ori;
    t = dot(p0l0, Plane_norm) / denom;
    bool intersectPlane = (t >= 0);
    newBuildingPlacement = Ray_ori + t * Ray_dir;
    //}
    std::cout << glm::to_string(newBuildingPlacement) << std::endl;

    return newBuildingPlacement;
}

//when a building is placed the desertMarker must be visible
void GameController::placeBuildingsOnLongLastingMarkers(std::vector<int> markerIDs, std::vector<mat4> pMat)
{
    //place buildings from newDetectedMarkers
    for (map<int, int>::iterator it = newDetectedMarkers.begin(); it != newDetectedMarkers.end() /* not hoisted */; /* no inc. */)
    {
        //TODO check if  worldReferenceMatrix is equal to the identity
        if (it->second >= 50)
        { // && (worldReferenceMatrix != Mat_<double>::eye(4, 4))) {
            //cout << MarkerID_Pmat_Map.find(it->first)->second << endl;

            //next Turn Marker detected
            if (it->first == nextTurnMarker)
            {
                cout << "Next turn" << endl;
                nextTurn();
            }
            else
            {
                for (int i = 0; i < markerIDs.size(); i++)
                {
                    if (markerIDs[i] == it->first)
                    {

                        mat4 relativeToWorldReference = pMat[i];
                        BuildingType newBuildingType = DefaultBuilding;
                        if (placeBuildingMarkers.contains(markerIDs[i]))
                            newBuildingType = placeBuildingMarkers.find(markerIDs[i])->second;

                        //glm::vec3 newBuildingLocation = getNewBuildingLocation(relativeToWorldReference);
                        //relativeToWorldReference = glm::mat4(1.0);
                        // relativeToWorldReference[3] = vec4(newBuildingLocation.x, newBuildingLocation.y, newBuildingLocation.z, 1.0);

                        GameBuilding newBuilding(currentPlayerID,
                            relativeToWorldReference,
                            newBuildingType,
                            buildingID);
                            buildingID++;
                           
                            pair<pair<int, int>, vec3> tpv = getTileAndPositionForNewBuilding(relativeToWorldReference, newBuildingType);
                            pair<int, int> tileAndPos = tpv.first;

                            //for testing building placement
                            newBuilding.relativeToWorldReference[3] = vec4(tpv.second, 1.0);

                            //offset y
                            newBuilding.relativeToWorldReference[3][1] = 0.015f;

                            if (checkCorrectBuildingPlacement(newBuilding, tileAndPos))
                            {
                                // place a Building in the terrain-array for use in the game logic and additionally in an allBuildings-Array
                                placeBuildingOrStreetonMap(tileAndPos.first, tileAndPos.second, newBuilding);
                                if (newBuilding.buildingType != City)
                                {
                                    allBuildings.push_back(newBuilding);
                                }
                            }
                    }
                }
                //cout << "relativeToWorldReference " << relativeToWorldReference << endl
            }
            it = newDetectedMarkers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

pair<pair<int, int>, glm::vec3> GameController::getTileAndPositionForNewBuilding(mat4 relativeToWorldReference, BuildingType newBuildingType)
{
    //TODO Jan
    //check just the translation in the two axis spanning the Map plane
    vec3 buildingLocation = getNewBuildingLocation(relativeToWorldReference);
    float tileSideDistance = 0.45;
    float tileHeight = 0.9;

    vec3 rightTile = vec3(0.0, 0.0, -0.078);
    vec3 rightUpTile = vec3(-0.068, 0.0, -0.038);
    vec3 leftUpTile = vec3(-0.068, 0.0, 0.038);
    vec3 leftTile = vec3(0.0, 0.0, 0.078);
    vec3 leftDownTile = vec3(0.068, 0.0, 0.038);
    vec3 rightDownTile = vec3(0.068, 0.0, -0.038);

    vector<vec3> allTilesFromOrigin = {
        2.0f * leftUpTile, leftUpTile + rightUpTile, 2.0f * rightUpTile,
        leftUpTile + leftTile, leftUpTile, rightUpTile, rightUpTile + rightTile,
        2.0f * leftTile, leftTile, vec3(0, 0, 0), rightTile, 2.0f * rightTile,
        leftDownTile + leftTile, leftDownTile, rightDownTile, rightDownTile + rightTile,
        2.0f * leftDownTile, leftDownTile + rightDownTile, 2.0f * rightDownTile};

    //measured on the rightTile of the desertMarker
    vec3 pos0 = vec3(-0.045, 0.0, 0);        //vec3(-0.046356 - rightTile.x, 0.0, 0.079186 - rightTile.z); //vec3(-0.046356, 0.0, 0.079186);
    vec3 pos1 = vec3(-0.0225, 0.0, -0.039); //vec3(-0.086774 - rightTile.x, 0.0, 0.076418 - rightTile.z); //vec3(-0.086774, 0.0, 0.076418);
    vec3 pos2 = vec3(0.0225, 0.0, -0.039);  //vec3(-0.0111148 - rightTile.x, 0.0, 0.041135 - rightTile.z); //vec3(-0.0111148, 0.0, 0.041135); //TODO remeasure
    vec3 pos3 = vec3(0.045, 0.0, 0);
    vec3 pos4 = vec3(0.0225, 0.0, 0.039);
    vec3 pos5 = vec3(-0.0225, 0.0, 0.039);

    vector<vec3> allPositionsFromTileOrigin = {pos0, pos1, pos2, pos3, pos4, pos5};

    vec3 translation(0.0);

    //check all possible Positions of Settlements/Cities or Roads; choose Location in a 0.005 radius around the Location (must be pretty accurate)
    pair<int, int> nearestLocation = make_pair(-1, -1);
    float smallest_distance = 1000000;
    switch (newBuildingType)
    {
    case Settlement:
    case City:
        for (int t = 0; t < allTilesFromOrigin.size(); t++)
        {
            for (int p = 0; p < allPositionsFromTileOrigin.size(); p++)
            {
                vec3 curVec = allTilesFromOrigin[t] + allPositionsFromTileOrigin[p];
                float distance = glm::distance(curVec, buildingLocation); //distance to placed building
                if (distance < smallest_distance)
                {
                    nearestLocation = make_pair(t, p);
                    smallest_distance = distance;
                    translation = allTilesFromOrigin[t] + allPositionsFromTileOrigin[p];
                }
            }
        }
        break;
    case Road:
        for (int t = 0; t < allTilesFromOrigin.size(); t++)
        {
            for (int p = 0; p < allPositionsFromTileOrigin.size(); p++)
            {
                vec3 curPosVector = allPositionsFromTileOrigin[p] + (allPositionsFromTileOrigin[(p + 1) % allPositionsFromTileOrigin.size()] - allPositionsFromTileOrigin[p]) / 2.0f;
                vec3 curVec = allTilesFromOrigin[t] + curPosVector;
                float distance = glm::distance(curVec, buildingLocation); //distance to placed building
                if (distance < smallest_distance)
                {
                    nearestLocation = make_pair(t, p);
                    smallest_distance = distance;
                    translation = allTilesFromOrigin[t] + curPosVector;
                }
            }
        }
        break;
    case DefaultBuilding:
        break;
    default:
        break;
    }

    cout << "placed building at t=" << nearestLocation.first << " and p=" << nearestLocation.second << endl;
    return make_pair(nearestLocation, translation);
}

bool GameController::checkCorrectBuildingPlacement(GameBuilding newBuilding, pair<int, int> tileAndPos)
{
    // for a settlement and road this method returns false if a building is already placed on the same position
    if (newBuilding.buildingType == Settlement)
    {
        map<int, GameBuilding>::iterator it = terrain[tileAndPos.first].buildingPlacement.find(tileAndPos.second);
        if (it != terrain[tileAndPos.first].buildingPlacement.end())
        {
            return false;
        }
        return allPlayers[currentPlayerID].payResourcesForSettlement();
    }
    else if (newBuilding.buildingType == Road)
    {
        map<int, GameBuilding>::iterator it = terrain[tileAndPos.first].streetPlacement.find(tileAndPos.second);
        if (it != terrain[tileAndPos.first].streetPlacement.end())
        {
            return false;
        }
        return allPlayers[currentPlayerID].payResourcesForRoad();
    }
    else
    {
        map<int, GameBuilding>::iterator it = terrain[tileAndPos.first].buildingPlacement.find(tileAndPos.second);
        if (it != terrain[tileAndPos.first].buildingPlacement.end())
        {
            if (it->second.buildingType != Settlement || it->second.playerID != currentPlayerID)
                return false;
            else { //upgrade Settlement to City
                if (allPlayers[currentPlayerID].payResourcesForCity()) {
                    
                    //int buildingID = terrain[tileAndPos.first].buildingPlacement.at(tileAndPos.second).buildingID;
                    cout << "id " << it->second.buildingID << endl;
                    for (int i = 0; i < allBuildings.size(); i++) {
                        if (allBuildings[i].buildingID == it->second.buildingID) {
                            allBuildings[i].buildingType = City;
                            cout << "payed resources" << endl;
                        }
                    }
                    //it->second.buildingType = City;
                    //update for all neighbours as well
                    for (int i = 0; i < 19; i++)//over all buildings in terrain-array
                    {
                        map<int, GameBuilding>::iterator it;
                        for (it = terrain[i].buildingPlacement.begin(); it != terrain[i].buildingPlacement.end(); it++)
                        {
                            GameBuilding curBuilding = it->second;
                            if (curBuilding.buildingID == it->second.buildingID)
                                curBuilding.buildingType = City;
                        }
                    }
                }
                else {
                    return false;
                }
            }
        }
    }
    //is a settlement already placed when a city is placed (update all representations in terrain and allBuildings)
    
    //TODO does it follow all the other necessary neighbour rules, etc...
}

void GameController::placeBuildingOrStreetonMap(int tile, int pos, GameBuilding building)
{
    /* Building Positions around a Tile
            0 
         5     1 
            T
         4     2 
            3
       Road Positions around a Tile ???
          5   0  
        4   T   1
          3   2 
    */
    //placeBuildingOrStreet(tile, pos, building);

    placeBuildingOrStreet(tile, pos, building);
    if (pos == 0)
    {
        Tile *topRightNeighbor = terrain[tile].topRightNeighbor;
        Tile *topLeftNeighbor = terrain[tile].topLeftNeighbor;
        if (topRightNeighbor != nullptr && topRightNeighbor->type != invisible)
        {
            //cout << "TR " << topRightNeighbor->type << endl;
            topRightNeighbor->placeBuildingOrStreet(2, building);
        }
        if (topLeftNeighbor != nullptr && topLeftNeighbor->type != invisible)
        {
            //cout << "TL " << topLeftNeighbor->type << endl;
            topLeftNeighbor->placeBuildingOrStreet(4, building);
        }
    }
    else if (pos == 1)
    {
        Tile *topRightNeighbor = terrain[tile].topRightNeighbor;
        Tile *rightNeighbor = terrain[tile].rightNeighbor;
        if (topRightNeighbor != nullptr && topRightNeighbor->type != invisible)
        {
            //cout << "TR " << topRightNeighbor->type << endl;
            topRightNeighbor->placeBuildingOrStreet(3, building);
        }
        if (rightNeighbor != nullptr && rightNeighbor->type != invisible)
        {
            //cout << "R " << rightNeighbor->type << endl;
            rightNeighbor->placeBuildingOrStreet(5, building);
        }
    }
    else if (pos == 2)
    {
        Tile *bottomRightNeighbor = terrain[tile].bottomRightNeighbor;
        Tile *rightNeighbor = terrain[tile].rightNeighbor;
        if (bottomRightNeighbor != nullptr && bottomRightNeighbor->type != invisible)
        {
            //cout << "BR " << bottomRightNeighbor->type << endl;
            bottomRightNeighbor->placeBuildingOrStreet(0, building);
        }
        if (rightNeighbor != nullptr && rightNeighbor->type != invisible)
        {
            //cout << "R " << rightNeighbor->type << endl;
            rightNeighbor->placeBuildingOrStreet(4, building);
        }
    }
    else if (pos == 3)
    {
        Tile *bottomRightNeighbor = terrain[tile].bottomRightNeighbor;
        Tile *bottomLeftNeighbor = terrain[tile].bottomLeftNeighbor;
        if (bottomRightNeighbor != nullptr && bottomRightNeighbor->type != invisible)
        {
            //cout << "BR " << bottomRightNeighbor->type << endl;
            bottomRightNeighbor->placeBuildingOrStreet(5, building);
        }
        if (bottomLeftNeighbor != nullptr && bottomLeftNeighbor->type != invisible)
        {
            //cout << "BL " << bottomLeftNeighbor->type << endl;
            bottomLeftNeighbor->placeBuildingOrStreet(1, building);
        }
    }
    else if (pos == 4)
    {
        Tile *leftNeighbor = terrain[tile].leftNeighbor;
        Tile *bottomLeftNeighbor = terrain[tile].bottomLeftNeighbor;
        if (leftNeighbor != nullptr && leftNeighbor->type != invisible)
        {
            //cout << "L " << leftNeighbor->type << endl;
            leftNeighbor->placeBuildingOrStreet(2, building);
        }
        if (bottomLeftNeighbor != nullptr && bottomLeftNeighbor->type != invisible)
        {
            //cout << "BL" << bottomLeftNeighbor->type << endl;
            bottomLeftNeighbor->placeBuildingOrStreet(0, building);
        }
    }
    else if (pos == 5)
    {
        Tile *leftNeighbor = terrain[tile].leftNeighbor;
        Tile *topLeftNeighbor = terrain[tile].topLeftNeighbor;
        if (leftNeighbor != nullptr && leftNeighbor->type != invisible)
        {
            //cout << "L " << leftNeighbor->type << endl;
            leftNeighbor->placeBuildingOrStreet(1, building);
        }
        if (topLeftNeighbor != nullptr && topLeftNeighbor->type != invisible)
        {
            //cout << "TL " << topLeftNeighbor->type << endl;
            topLeftNeighbor->placeBuildingOrStreet(3, building);
        }
    }
    //update Points of current Player
    if (building.buildingType == Settlement) {
        allPlayers[currentPlayerID].addPoints(1);
    }
    else if (building.buildingType == City) {
        allPlayers[currentPlayerID].addPoints(1); //no upgrade just placed it
    }
}

void GameController::placeBuildingOrStreet(int tile, int pos, GameBuilding building)
{
    if (building.buildingType == Settlement || building.buildingType == City)
    {
        terrain[tile].buildingPlacement[pos] = building;
    }
    else
    {
        terrain[tile].streetPlacement[pos] = building;
    }
}

void GameController::assignNeighbors()
{
    /* Nummerierung
        0 | 1 | 2
       3 | 4 | 5 | 6
    7 | 8 | 9 | 10 | 11
     12 | 13 | 14 | 15
       16 | 17 | 18
    */

    // assign left and right neighbors
    for (int i = 0; i < 19; i++)
    {
        if (i == 0 || i == 3 || i == 7 || i == 12 || i == 16)
        {
            continue;
        }
        terrain[i].leftNeighbor = &terrain[i - 1];
        terrain[i - 1].rightNeighbor = &terrain[i];

    }

    //assign topLeft and bottomRight neighbors
    int offset;
    for (int i = 3; i < 19; i++)
    {
        if (i == 3 || i == 7)
        {
            continue;
        }
        if (i > 6 && i < 16)
        {
            offset = 5;
        }
        else
        {
            offset = 4;
        }
        terrain[i].topLeftNeighbor = &terrain[i - offset];
        terrain[i - offset].bottomRightNeighbor = &terrain[i];
    }

    // assign topRight and bottomLeft neighbors
    for (int i = 3; i < 19; i++)
    {
        if (i == 6 || i == 11)
        {
            continue;
        }
        if (i > 6 && i < 16)
        {
            offset = 4;
        }
        else
        {
            offset = 3;
        }
        terrain[i].topRightNeighbor = &terrain[i - offset];
        terrain[i - offset].bottomLeftNeighbor = &terrain[i];
    }
}

mat4 GameController::getWorldReferenceMarker(std::vector<int> &MarkerID, std::vector<glm::mat4> &MarkerPmat)
{
    //use desert-marker as only world-reference (expandable...)
    for (int i = 0; i < MarkerID.size(); i++)
    {
        if (MarkerID[i] == desertMarker)
        {
            desertMarkerLostFrames = 0;
            desertMarkerPose = MarkerPmat[i];
            return desertMarkerPose;
        }
    }

    desertMarkerLostFrames++;
    if (desertMarkerLostFrames > 5)
    {
        return glm::mat4(1.0);
    }
    else
    {
        return desertMarkerPose;
    }
}

void GameController::halvePlayerRessources()
{

    int combined;
    for (int i = 0; i < 2; i++)
    {
        allPlayers[i].halveResources();
    }
}

void GameController::placeSettlementAndStreetAtStartOfGame(int playerID, mat4 pos)
{
    GameBuilding settlement(playerID, pos, Settlement, buildingID);
    buildingID++;
    placeBuildingOrStreetonMap(0, 0, settlement);
    GameBuilding road(playerID, pos, Road, buildingID);
    buildingID++;
    placeBuildingOrStreetonMap(0, 0, road);
}

void GameController::collectPlayerRessources(int dice)
{
    for (int i = 0; i < 19; i++)
    {
        map<int, GameBuilding>::iterator it;
        for (it = terrain[i].buildingPlacement.begin(); it != terrain[i].buildingPlacement.end(); it++)
        {
            if (dice == -1 || dice == terrain[i].diceNumber)
            {
                int playerID = it->second.playerID;
                BuildingType bT = it->second.buildingType;
                int ressourceAmount = 0;
                switch (bT)
                {
                case Road:
                    break;
                case Settlement:
                    ressourceAmount = 1;
                    break;
                case City:
                    ressourceAmount = 2;
                    break;
                case DefaultBuilding:
                    break;
                default:
                    break;
                }
                if (terrain[i].type == hills)
                {
                    allPlayers[playerID].addResource(brick, ressourceAmount);
                }
                else if (terrain[i].type == forest)
                {
                    allPlayers[playerID].addResource(lumber, ressourceAmount);
                }
                else if (terrain[i].type == mountains)
                {
                    allPlayers[playerID].addResource(ore, ressourceAmount);
                }
                else if (terrain[i].type == fields)
                {
                    allPlayers[playerID].addResource(grain, ressourceAmount);
                }
                else if (terrain[i].type == pastures)
                {
                    allPlayers[playerID].addResource(wool, ressourceAmount);
                }
            }
        }
    }
}

/*
void GameController::addResourceToPlayer(int playerID, int tile, int amount)
{
}*/

int GameController::gameOver()
{
    if (allPlayers[0].points == 10)
    {
        return 0;
    }
    else if (allPlayers[1].points == 10)
    {
        return 1;
    }
    return -1;
}
