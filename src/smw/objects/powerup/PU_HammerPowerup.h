#pragma once

#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_HammerPowerup : public MO_Powerup {
public:
    PU_HammerPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    bool collide(CPlayer* player) override;
};
