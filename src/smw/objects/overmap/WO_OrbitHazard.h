#pragma once

#include "OverMapObject.h"


class OMO_OrbitHazard : public IO_OverMapObject {
public:
    OMO_OrbitHazard(gfxSprite* nspr, Vec2s pos, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth);

    void update() override;

    bool collide(CPlayer*) override;

private:
    void CalculatePosition();

    float dAngle, dVel, dRadius;
    Vec2f dCenter;
};
