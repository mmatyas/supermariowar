#pragma once

#include "OverMapObject.h"


class OMO_StraightPathHazard : public IO_OverMapObject {
public:
    OMO_StraightPathHazard(gfxSprite* nspr, Vec2s pos, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);

    void update() override;

    bool collide(CPlayer*) override;

private:
    float dAngle, dVel;
};
