#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_JailKeyPowerup : public MO_Powerup {
public:
    PU_JailKeyPowerup(gfxSprite* nspr, Vec2s pos);

    bool collide(CPlayer* player) override;
};
