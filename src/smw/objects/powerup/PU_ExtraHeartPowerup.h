#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ExtraHeartPowerup : public MO_Powerup {
public:
    PU_ExtraHeartPowerup(gfxSprite* nspr, short x, short y);
    ~PU_ExtraHeartPowerup() {};

    bool collide(CPlayer* player) override;
};
