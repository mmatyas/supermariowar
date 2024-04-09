#pragma once

#include "CO_Spring.h"

class CO_Spike : public CO_Spring {
public:
    CO_Spike(gfxSprite* nspr, short ix, short iy);

private:
    void hittop(CPlayer* player);
};
