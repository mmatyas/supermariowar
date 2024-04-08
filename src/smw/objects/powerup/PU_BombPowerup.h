#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_BombPowerup : public MO_Powerup {
public:
    PU_BombPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
    ~PU_BombPowerup() {};

    bool collide(CPlayer* player) override;
};
