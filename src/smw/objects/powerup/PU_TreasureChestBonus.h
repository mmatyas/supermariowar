#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_TreasureChestBonus : public MO_Powerup {
public:
    PU_TreasureChestBonus(gfxSprite* nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem);
    ~PU_TreasureChestBonus() {};

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    float BottomBounce() override;

private:
    short sparkleanimationtimer;
    short sparkledrawframe;
    short numbounces;
    short bonusitem;
    short drawbonusitemx, drawbonusitemy;
    short drawbonusitemtimer;
};
