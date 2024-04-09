#pragma once

#include "objects/moving/MO_CarriedObject.h"

class CPlayer;
class gfxSprite;

class CO_Egg : public MO_CarriedObject {
public:
    CO_Egg(gfxSprite* nspr, short iColor);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    void placeEgg();
    void Drop() override;

    short getColor() const { return color; }

private:
    short relocatetimer;
    short explosiondrawframe, explosiondrawtimer;

    CPlayer* owner_throw;
    short owner_throw_timer;

    short sparkleanimationtimer;
    short sparkledrawframe;

    short color;

    short egganimationrates[6];

friend class MO_Yoshi;
friend class CGM_Eggs;
};
