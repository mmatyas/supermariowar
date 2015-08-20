#include "ObjectContainer.h"

#include "GameValues.h"
#include "object.h" // MovingObject
#include "objectgame.h"
#include "RandomNumberGenerator.h"

#include <cmath>

extern CGameValues game_values;

//------------------------------------------------------------------------------
// class object_container
//------------------------------------------------------------------------------
CObjectContainer::CObjectContainer()
{
    for (short i = 0; i < MAXOBJECTS; i++)
        list[i] = NULL;

    list_end = 0;
}

CObjectContainer::~CObjectContainer()
{
    clean();
}

void CObjectContainer::clean()
{
    for (short i = 0; i < list_end; i++) {
        delete list[i];
        list[i] = NULL;
    }
    list_end = 0;
}

bool CObjectContainer::add(CObject* ec)
{
    if (list_end >= MAXOBJECTS) {
        //printf("eyecandy list full!\n");
        delete ec;	// otherwise memory leak!
        return false;
    }

    list[list_end] = ec;
    ec->index = list_end;	// save index for removing
    list_end++;
    return true;
}

void CObjectContainer::update()
{
    for (short i = 0; i < list_end; i++)
        list[i]->update();
}

void CObjectContainer::draw()
{
    for (short i = list_end - 1; i >= 0; i--)
        list[i]->draw();
}

bool CObjectContainer::isBlockAt(short x, short y)
{
    for (short i = 0; i < list_end; i++) {
        if (x >= list[i]->ix && x < list[i]->ix + list[i]->iw &&
                y >= list[i]->iy && y < list[i]->iy + list[i]->ih &&
                list[i]->getObjectType() == object_block) {
            return true;
        }
    }

    return false;
}

float CObjectContainer::getClosestObject(short ix, short iy, short objectType)
{
    int dist = smw->ScreenWidth * 1000;  //Longest distance from corner to corner squared

    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() != objectType)
            continue;

        short x = list[i]->ix - ix;
        short y = list[i]->iy - iy;

        int calcdist = x * x + y * y;

        if (calcdist < dist)
            dist = calcdist;
    }

    return (float)sqrt((double)dist);
}

float CObjectContainer::getClosestMovingObject(short ix, short iy, short movingObjectType)
{
    int dist = smw->ScreenWidth * 1000;  //Longest distance from corner to corner squared

    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() != object_moving || ((IO_MovingObject*)list[i])->getMovingObjectType() != movingObjectType)
            continue;

        short x = list[i]->ix - ix;
        short y = list[i]->iy - iy;

        int calcdist = x * x + y * y;

        if (calcdist < dist)
            dist = calcdist;
    }

    return (float)sqrt((double)dist);
}

short CObjectContainer::countTypes(ObjectType type)
{
    short count = 0;

    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() == type) {
            count++;
        }
    }

    return count;
}

short CObjectContainer::countMovingTypes(MovingObjectType type)
{
    short count = 0;

    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() == object_moving && ((IO_MovingObject*)list[i])->getMovingObjectType() == type) {
            count++;
        }
    }

    return count;
}

void CObjectContainer::adjustPlayerAreas(CPlayer * player, CPlayer * other)
{
    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() == object_area) {
            OMO_Area * area = (OMO_Area*)list[i];

            if (area->colorID == other->colorID) {
                if (game_values.gamemodesettings.domination.relocateondeath)
                    area->placeArea();

                if (game_values.gamemodesettings.domination.stealondeath && player)
                    area->setOwner(player);
                else if (game_values.gamemodesettings.domination.loseondeath)
                    area->reset();
            }
        }
    }
}

void CObjectContainer::removePlayerRaceGoals(short id, short iGoal)
{
    if (game_values.gamemodesettings.race.penalty == 0 && iGoal != -1)
        return;

    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() == object_race_goal) {
            OMO_RaceGoal * goal = (OMO_RaceGoal*)list[i];

            if (iGoal == -1 || 2 == game_values.gamemodesettings.race.penalty ||
                    (1 == game_values.gamemodesettings.race.penalty && goal->getGoalID() == iGoal)) {
                goal->reset(id);
            }
        }
    }
}

void CObjectContainer::pushBombs(short x, short y)
{
    for (short i = 0; i < list_end; i++) {
        if (list[i]->getObjectType() != object_moving)
            continue;

        IO_MovingObject * mo = (IO_MovingObject*)list[i];

        if (mo->getMovingObjectType() != movingobject_bomb)
            continue;

        CO_Bomb * bomb = (CO_Bomb*)list[i];

        if (bomb->HasOwner())
            continue;

        int bombx = bomb->ix + (bomb->iw >> 1) - x;
        int bomby = bomb->iy + (bomb->ih >> 1) - y;

        int dist = bombx * bombx + bomby * bomby;

        if (dist < 10000) {
            if (bombx > 0)
                bomb->velx += ((float)(RANDOM_INT(30)) / 10.0f + 4.0f);
            else
                bomb->velx -= ((float)(RANDOM_INT(30)) / 10.0f + 4.0f);

            bomb->vely -= (float)(RANDOM_INT(30)) / 10.0f + 6.0f;
        }
    }
}

void CObjectContainer::cleandeadobjects()
{
    for (short i = 0; i < list_end; i++) {
        if (list[i]->dead) {
            delete list[i];
            list_end--;

            if (i != list_end) {
                list[i] = list[list_end];
            }

            i--;
        }
    }
}

CObject * CObjectContainer::getRandomObject()
{
    if (list_end == 0)
        return NULL;

    return list[RANDOM_INT( list_end)];
}
