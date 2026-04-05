#pragma once

#include "OverMapObject.h"

class OMO_PipeCoin : public IO_OverMapObject {
public:
    OMO_PipeCoin(gfxSprite* nspr, Vec2f vel, Vec2s pos, short teamid, short colorid, short uncollectabletime);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    short GetColor() const { return iColorID; }
    short GetTeam() const { return iTeamID; }

private:
    const short iTeamID, iColorID;

    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;

    short iUncollectableTime;
};
