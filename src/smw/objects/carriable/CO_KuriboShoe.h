#pragma once

#include "CO_Spring.h"

class CO_KuriboShoe : public CO_Spring {
public:
    CO_KuriboShoe(gfxSprite* nspr, Vec2s pos, bool fSticky);

private:
    void hittop(CPlayer* player);

    bool fSticky;
};
