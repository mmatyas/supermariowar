#pragma once

#include "MovingObject.h"


class MO_Yoshi : public IO_MovingObject {
public:
    MO_Yoshi(gfxSprite* nspr, short iColor);

    void update() override;
    bool collide(CPlayer* player) override;
    void collide(IO_MovingObject* object) override;
    void placeYoshi();
    short getColor()
    {
        return color;
    }

private:
    short timer;
    short color;
};
