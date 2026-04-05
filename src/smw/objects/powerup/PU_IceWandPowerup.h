#pragma once

#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_IceWandPowerup : public MO_Powerup {
public:
    PU_IceWandPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

private:
    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
};
