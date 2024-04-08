#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_CoinPowerup : public MO_Powerup {
public:
    PU_CoinPowerup(gfxSprite* nspr, short x, short y, short color, short value);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

protected:
    const short iColorOffsetY;
    const short iValue;

    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
};
