#pragma once

#include "MovingObjectTypes.h"
#include "ObjectBase.h"

#include <memory>
#include <vector>


//object container
class CObjectContainer {
public:
    CObjectContainer();

    bool add(CObject* cio);
    void update() const;
    void draw() const;

    void clean();
    void cleanDeadObjects();

    float getClosestObject(short x, short y, ObjectType objectType) const;
    float getClosestMovingObject(short x, short y, MovingObjectType movingObjectType) const;
    size_t countTypes(ObjectType type) const;
    size_t countMovingTypes(MovingObjectType type) const;

    const std::vector<std::unique_ptr<CObject>>& list() const { return m_list; }

private:
    std::vector<std::unique_ptr<CObject>> m_list;
};
