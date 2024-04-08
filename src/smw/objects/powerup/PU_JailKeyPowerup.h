#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_JailKeyPowerup : public MO_Powerup {
public:
    PU_JailKeyPowerup(gfxSprite* nspr, short x, short y);
    ~PU_JailKeyPowerup() {};

    bool collide(CPlayer* player) override;
};
