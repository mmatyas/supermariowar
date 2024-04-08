#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_PodoboPowerup : public MO_Powerup {
public:
    PU_PodoboPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    bool collide(CPlayer* player) override;
};
