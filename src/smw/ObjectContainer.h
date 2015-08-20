#ifndef OBJECTCONTAINER_H
#define OBJECTCONTAINER_H

#include "MovingObjectTypes.h"
#include "ObjectBase.h"
#include "player.h"

//object container
class CObjectContainer
{
    public:
        CObjectContainer();
        ~CObjectContainer();

        bool add(CObject* cio);
        void update();
        void draw();
        void clean();

        bool isBlockAt(short x, short y);

        void cleandeadobjects();

        float getClosestObject(short x, short y, short objectType);
        float getClosestMovingObject(short x, short y, short movingObjectType);
        short countTypes(ObjectType type);
        short countMovingTypes(MovingObjectType type);
        void adjustPlayerAreas(CPlayer * player, CPlayer * other);
        void removePlayerRaceGoals(short id, short iGoal);
        void pushBombs(short x, short y);

        CObject * getRandomObject();

    public:
        CObject *list[MAXOBJECTS];
        short        list_end;
};

#endif // OBJECTCONTAINER_H
