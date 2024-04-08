#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_PWingsPowerup : public MO_Powerup {
public:
    PU_PWingsPowerup(gfxSprite* nspr, short x, short y);

    bool collide(CPlayer* player) override;
};
