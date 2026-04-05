#pragma once

#include "MovingObject.h"


class MO_SuperFireball : public IO_MovingObject {
public:
    MO_SuperFireball(gfxSprite* nspr, Vec2s pos, short iNumSpr, Vec2f vel, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

private:
    short colorOffset;
    short directionOffset;
    short ttl;
};
