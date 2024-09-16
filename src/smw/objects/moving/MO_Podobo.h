#pragma once

#include "MovingObject.h"


class MO_Podobo : public IO_MovingObject {
public:
    MO_Podobo(gfxSprite* nspr, Vec2s pos, float nspeed, short playerid, short teamid, short colorid, bool isSpawned);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject* object) override;

private:
    short iColorOffsetY;

    bool fIsSpawned;
    short iHiddenPlane;
};
