#pragma once

#include "OverMapObject.h"

class OMO_BowserFire : public IO_OverMapObject {
public:
    OMO_BowserFire(gfxSprite* nspr, short x, short y, float velx, float vely,
        short id, short teamid, short colorid);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

private:
    const short iPlayerID;
    const short iTeamID;
    const short iColorOffsetY;
};
