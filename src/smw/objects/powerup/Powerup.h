#pragma once

#include "objects/moving/MovingObject.h"


class MO_Powerup : public IO_MovingObject {
public:
    MO_Powerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1);

    void draw() override;
    void update() override;
    bool collide(CPlayer* player) override;

    virtual void nospawn(short y);

protected:
    float desty;
};
