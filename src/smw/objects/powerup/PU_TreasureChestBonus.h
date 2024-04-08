#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_TreasureChestBonus : public MO_Powerup {
public:
    PU_TreasureChestBonus(gfxSprite* nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    float BottomBounce() override;

private:
    const short bonusitem;
    short numbounces = 5;
    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
    short drawbonusitemx = 0;
    short drawbonusitemy = 0;
    short drawbonusitemtimer = 0;
};
