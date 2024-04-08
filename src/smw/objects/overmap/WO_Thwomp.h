#pragma once

#include "OverMapObject.h"

class OMO_Thwomp : public IO_OverMapObject {
public:
    OMO_Thwomp(gfxSprite* nspr, short x, float nspeed);

    void update() override;
    bool collide(CPlayer* player) override;
};
