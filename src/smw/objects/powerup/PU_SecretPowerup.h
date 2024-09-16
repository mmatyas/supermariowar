#pragma once


#include "objects/powerup/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_SecretPowerup : public MO_Powerup {
public:
    PU_SecretPowerup(gfxSprite* nspr, Vec2s pos, short type);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    void place();

private:
    const short itemtype;  // TODO: enum
    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
};
