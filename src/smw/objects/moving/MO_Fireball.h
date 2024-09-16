#pragma once

#include "MovingObject.h"

class Spotlight;


class MO_Fireball : public IO_MovingObject {
public:
    MO_Fireball(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

    short colorOffset;

private:
    short ttl;
    Spotlight* sSpotlight;
};
