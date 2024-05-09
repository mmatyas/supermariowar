#include "ObjectContainer.h"

#include "Game.h"
#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "objects/carriable/CO_Bomb.h"
#include "objects/overmap/WO_Area.h"
#include "objects/overmap/WO_RaceGoal.h"

#include <algorithm>
#include <cmath>

extern CGameValues game_values;

//------------------------------------------------------------------------------
// class object_container
//------------------------------------------------------------------------------
CObjectContainer::CObjectContainer()
{
    m_list.reserve(MAXOBJECTS);
}

void CObjectContainer::clean()
{
    m_list.clear();
}

void CObjectContainer::cleanDeadObjects()
{
    auto erase_from = std::remove_if(m_list.begin(), m_list.end(),
        [](const std::unique_ptr<CObject>& obj) { return obj->isDead(); });
    m_list.erase(erase_from, m_list.end());
}

bool CObjectContainer::add(CObject* ptr)
{
    if (m_list.size() + 1 >= m_list.capacity()) {
        //printf("eyecandy list full!\n");
        delete ptr;	// otherwise memory leak!
        return false;
    }
    m_list.emplace_back(ptr);
    return true;
}

void CObjectContainer::update() const
{
    for (const std::unique_ptr<CObject>& obj : m_list)
        obj->update();
}

void CObjectContainer::draw() const
{
    for (const std::unique_ptr<CObject>& obj : m_list)
        obj->draw();
}

float CObjectContainer::getClosestObject(short ix, short iy, ObjectType objectType) const
{
    float minDist = App::screenWidth * 1000;  //Longest distance from corner to corner squared

    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() != objectType)
            continue;

        short x = obj->ix - ix;
        short y = obj->iy - iy;

        float dist = x * x + y * y;
        if (dist < minDist)
            minDist = dist;
    }

    return ::sqrtf(minDist);
}

float CObjectContainer::getClosestMovingObject(short ix, short iy, MovingObjectType movingObjectType) const
{
    float minDist = App::screenWidth * 1000;  //Longest distance from corner to corner squared

    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() != object_moving || ((IO_MovingObject*)obj.get())->getMovingObjectType() != movingObjectType)
            continue;

        short x = obj->ix - ix;
        short y = obj->iy - iy;

        float dist = x * x + y * y;
        if (dist < minDist)
            minDist = dist;
    }

    return ::sqrtf(minDist);
}

size_t CObjectContainer::countTypes(ObjectType type) const
{
    size_t count = 0;

    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() == type) {
            count++;
        }
    }

    return count;
}

size_t CObjectContainer::countMovingTypes(MovingObjectType type) const
{
    size_t count = 0;

    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() == object_moving && ((IO_MovingObject*)obj.get())->getMovingObjectType() == type) {
            count++;
        }
    }

    return count;
}

void CObjectContainer::adjustPlayerAreas(CPlayer* player, CPlayer* other) const
{
    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() != object_area)
            continue;

        auto* area = static_cast<OMO_Area*>(obj.get());
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

void CObjectContainer::removePlayerRaceGoals(short id, short iGoal) const
{
    if (game_values.gamemodesettings.race.penalty == 0 && iGoal != -1)
        return;

    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() != object_race_goal)
            continue;

        auto* goal = static_cast<OMO_RaceGoal*>(obj.get());
        if (iGoal == -1 || 2 == game_values.gamemodesettings.race.penalty ||
                (1 == game_values.gamemodesettings.race.penalty && goal->getGoalID() == iGoal)) {
            goal->reset(id);
        }
    }
}

void CObjectContainer::pushBombs(short x, short y) const
{
    for (const std::unique_ptr<CObject>& obj : m_list) {
        if (obj->getObjectType() != object_moving)
            continue;

        auto* mo = static_cast<IO_MovingObject*>(obj.get());
        if (mo->getMovingObjectType() != movingobject_bomb)
            continue;

        auto* bomb = static_cast<CO_Bomb*>(obj.get());
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

CObject* CObjectContainer::getRandomObject() const
{
    return m_list.empty()
        ? nullptr
        : m_list[RANDOM_INT(m_list.size())].get();
}
