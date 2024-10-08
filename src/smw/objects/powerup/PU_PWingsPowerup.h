#pragma once


#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_PWingsPowerup : public MO_Powerup {
public:
    PU_PWingsPowerup(gfxSprite* nspr, Vec2s pos);

    bool collide(CPlayer* player) override;
};
