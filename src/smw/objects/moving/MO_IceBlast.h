#pragma once

#include "MovingObject.h"

class Spotlight;


class MO_IceBlast : public IO_MovingObject {
public:
    MO_IceBlast(gfxSprite* nspr, Vec2s pos, float fVelyX, short iGlobalID, short iTeamID, short iColorID);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

private:
    short ttl;
    Spotlight* sSpotlight;
};
