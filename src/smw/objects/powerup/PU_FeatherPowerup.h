#pragma once

#include "GlobalConstants.h"
#include "objects/moving/MovingObject.h"

class CPlayer;
class gfxSprite;


class PU_FeatherPowerup : public IO_MovingObject {
public:
    PU_FeatherPowerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);

    void update() override;
    void draw() override;
    bool collide(CPlayer* player) override;

    void nospawn(short y);

private:
    bool fFloatDirectionRight = true;
    float dFloatAngle = HALF_PI;
    float dFloatCenterX = 0.f;
    float dFloatCenterY = 0.f;
    float desty;
};
