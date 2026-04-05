#pragma once


#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_Tanooki : public MO_Powerup {
public:
    PU_Tanooki(Vec2s pos);

    bool collide(CPlayer* player) override;
};
