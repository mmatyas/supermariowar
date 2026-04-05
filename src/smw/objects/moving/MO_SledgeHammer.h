#pragma once

#include "MovingObject.h"


class MO_SledgeHammer : public IO_MovingObject {
public:
    MO_SledgeHammer(gfxSprite* nspr, Vec2s pos, short iNumSpr, Vec2f vel, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);

    void update() override;
    bool collide(CPlayer* player) override;
    void explode();
    void draw() override;

    short playerID;
    short teamID;
    short colorOffset;

private:
    bool fSuper;
};
