#pragma once

#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_BombPowerup : public MO_Powerup {
public:
    PU_BombPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    bool collide(CPlayer* player) override;
};
