#include "Player.h"
#include <array>
#include <iostream>

void Player::addResource(RessourceType resourceType, int amount)
{
    switch (resourceType)
    {
    case RessourceType::brick:
        resources[0] += amount;
        break;
    case RessourceType::lumber:
        resources[1] += amount;
        break;
    case RessourceType::ore:
        resources[2] += amount;
        break;
    case RessourceType::grain:
        resources[3] += amount;
        break;
    case RessourceType::wool:
        resources[4] += amount;
        break;
    case nothing:
        std::cout << "nothing is added to the players ressources" << std::endl;
        break;
    default:
        std::cout << "no existing RessourceType for the player to add to" << std::endl;
        break;
    }
}

void Player::addPoints(int amount)
{
    points += amount;
}

void Player::halveResources()
{
    int combined = 0;
    for (int i = 0; i < 5; i++)
    {
        combined += resources[i];
    }

    if (combined > 7)
    {
        combined /= 2;
        int i = 0;
        while (combined > 0)
        {
            if (i == 4)
            {
                i = 0;
            }
            if (resources[i] > 0) {
                resources[i]--;
                combined--;
            }
            i++;
        }
    }
}

bool Player::payResourcesForRoad()
{
    if (resources[0] > 0 && resources[1] > 0)
    {
        resources[0]--;
        resources[1]--;
        return true;
    }
    return false;
}

bool Player::payResourcesForSettlement()
{
    for (int i = 0; i < 5; i++)
    {
        if (i == 2)
        {
            continue;
        }
        if (resources[i] < 1)
        {
            return false;
        }
    }
    resources[0]--;
    resources[1]--;
    resources[3]--;
    resources[4]--;
    return true;
}

bool Player::payResourcesForCity()
{
    if (resources[2] > 3 && resources[3] > 2)
    {
        resources[2] -= 3;
        resources[3] -= 2;
        return true;
    }
    return false;
}