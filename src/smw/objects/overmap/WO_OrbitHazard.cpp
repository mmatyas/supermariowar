#include "WO_OrbitHazard.h"

#include "player.h"
#include "ResourceManager.h"

#include <cmath>

extern CResourceManager* rm;


//------------------------------------------------------------------------------
// class OMO Orbit Hazard - component of the fireball string or rotodisc
//------------------------------------------------------------------------------
OMO_OrbitHazard::OMO_OrbitHazard(gfxSprite* nspr, short x, short y, float radius, float vel, float angle, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth)
    : IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    objectType = object_orbithazard;

    dRadius = radius;
    dVel = vel;
    dAngle = angle;

    dCenterX = (float)x;
    dCenterY = (float)y;

    CalculatePosition();
}

void OMO_OrbitHazard::update()
{
    animate();

    dAngle += dVel;

    if (dAngle < 0.0f)
        dAngle += TWO_PI;
    else if (dAngle >= TWO_PI)
        dAngle -= TWO_PI;

    CalculatePosition();
}

bool OMO_OrbitHazard::collide(CPlayer* player)
{
    if (!player->isInvincible() && !player->isShielded() && !player->shyguy) {
        return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
    }

    return false;
}

void OMO_OrbitHazard::CalculatePosition()
{
    setXf(dCenterX + dRadius * cos(dAngle) - (float)iw / 2.0f);
    setYf(dCenterY + dRadius * sin(dAngle) - (float)ih / 2.0f);
}
