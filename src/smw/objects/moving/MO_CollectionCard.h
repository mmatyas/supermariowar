#pragma once

#include "MovingObject.h"


class MO_CollectionCard : public IO_MovingObject {
public:
    MO_CollectionCard(gfxSprite* nspr, short iType, short iValue, short iUncollectableTime, Vec2f vel, Vec2s pos);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    void placeCard();

    short getType()
    {
        return type;
    }
    short getValue()
    {
        return value;
    }

private:
    short timer;
    short type;
    short value;

    short sparkleanimationtimer;
    short sparkledrawframe;

    short uncollectabletime;
};
