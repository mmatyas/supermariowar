#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_Tanooki : public MO_Powerup {
public:
    PU_Tanooki(short x, short y);
    bool collide(CPlayer* player);
};
