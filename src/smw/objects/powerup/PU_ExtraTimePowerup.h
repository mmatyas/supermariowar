#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ExtraTimePowerup : public MO_Powerup {
public:
    PU_ExtraTimePowerup(gfxSprite* nspr, Vec2s pos);

    bool collide(CPlayer* player) override;
};
