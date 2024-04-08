#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_FirePowerup : public MO_Powerup {
public:
    PU_FirePowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    bool collide(CPlayer* player) override;
};
