#pragma once

#include "CO_Spring.h"

class CO_KuriboShoe : public CO_Spring {
public:
    CO_KuriboShoe(gfxSprite* nspr, short ix, short iy, bool fSticky);

private:
    void hittop(CPlayer* player);

    bool fSticky;
};
