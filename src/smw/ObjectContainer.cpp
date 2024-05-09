#include "ObjectContainer.h"

#include "objects/moving/MovingObject.h"

#include <algorithm>
#include <limits>
#include <cmath>

#define MAXOBJECTS  300

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
    float minDist = std::numeric_limits<float>::max();

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
    float minDist = std::numeric_limits<float>::max();

    for (const std::unique_ptr<CObject>& obj : m_list) {
        auto* movobj = dynamic_cast<IO_MovingObject*>(obj.get());
        if (!movobj || movobj->getMovingObjectType() != movingObjectType)
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
        auto* movobj = dynamic_cast<IO_MovingObject*>(obj.get());
        if (movobj && movobj->getMovingObjectType() == type) {
            count++;
        }
    }

    return count;
}
