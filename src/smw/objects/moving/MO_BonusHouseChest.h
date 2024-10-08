#pragma once

#include "MovingObject.h"


class MO_BonusHouseChest : public IO_MovingObject {
public:
    MO_BonusHouseChest(gfxSprite* nspr, Vec2s pos, short iBonusItem);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

private:
    short bonusitem;
    short drawbonusitemy;
    short drawbonusitemtimer;
};
