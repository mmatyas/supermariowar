#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ExtraHeartPowerup : public MO_Powerup {
public:
    PU_ExtraHeartPowerup(gfxSprite* nspr, Vec2s pos);

    bool collide(CPlayer* player) override;
};
