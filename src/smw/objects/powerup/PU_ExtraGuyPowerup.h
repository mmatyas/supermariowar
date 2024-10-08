#pragma once

#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ExtraGuyPowerup : public MO_Powerup {
public:
    PU_ExtraGuyPowerup(gfxSprite* nspr, Vec2s pos, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type);

    bool collide(CPlayer* player) override;

private:
    const short iType;
};
