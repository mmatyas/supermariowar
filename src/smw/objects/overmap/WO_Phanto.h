#pragma once

#include "OverMapObject.h"

class OMO_Phanto : public IO_OverMapObject {
public:
    OMO_Phanto(gfxSprite* nspr, Vec2s pos, float velx, float vely, short type);

    void update() override;
    bool collide(CPlayer* player) override;

    short GetType() const { return iType; }

private:
    const short iType;

    float dMaxSpeedX, dMaxSpeedY;
    float dReactionSpeed, dSpeedRatio;
    short iSpeedTimer = 0;
};
