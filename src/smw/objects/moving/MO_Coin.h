#pragma once

#include "MovingObject.h"


class gfxSprite;
class CPlayer;

class MO_Coin : public IO_MovingObject {
public:
    MO_Coin(gfxSprite* nspr, float velx, float vely, short ix, short iy, short color, short team, short type, short uncollectabletime, bool placecoin);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;
    void placeCoin();

private:
    short timer;
    short sparkleanimationtimer;
    short sparkledrawframe;

    short iType;
    short iTeam;
    short iUncollectableTime;
};
