#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_SecretPowerup : public MO_Powerup {
public:
    PU_SecretPowerup(gfxSprite* nspr, short x, short y, short type);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    void place();

private:
    const short itemtype;  // TODO: enum
    short sparkleanimationtimer = 0;
    short sparkledrawframe = 0;
};
