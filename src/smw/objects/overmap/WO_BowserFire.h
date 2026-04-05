#pragma once

#include "OverMapObject.h"

class OMO_BowserFire : public IO_OverMapObject {
public:
    OMO_BowserFire(gfxSprite* nspr, Vec2s pos, Vec2f vel, short id, short teamid, short colorid);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

private:
    const short iPlayerID;
    const short iTeamID;
    const short iColorOffsetY;
};
