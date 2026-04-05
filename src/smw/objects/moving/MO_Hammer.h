#pragma once

#include "MovingObject.h"

class Spotlight;


class MO_Hammer : public IO_MovingObject {
public:
    MO_Hammer(gfxSprite* nspr, Vec2s pos, short iNumSpr, Vec2f vel, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);

    void update() override;
    bool collide(CPlayer* player) override;
    void draw() override;

    short colorOffset;

private:
    short ttl;
    bool fSuper;
    Spotlight* sSpotlight;
};
