#pragma once

#include "MovingObject.h"


class MO_SuperFireball : public IO_MovingObject {
public:
    MO_SuperFireball(gfxSprite* nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

private:
    short colorOffset;
    short directionOffset;
    short ttl;
};
