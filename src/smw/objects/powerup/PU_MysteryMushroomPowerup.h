#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_MysteryMushroomPowerup : public MO_Powerup {
public:
    PU_MysteryMushroomPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
    ~PU_MysteryMushroomPowerup() {};

    bool collide(CPlayer* player) override;
};
