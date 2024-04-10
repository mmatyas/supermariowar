#include "WO_StraightPathHazard.h"

#include "eyecandy.h"
#include "Game.h"
#include "player.h"
#include "ResourceManager.h"

#include <cmath>

extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class OMO Straight Path Hazard - straight path fireball
//------------------------------------------------------------------------------
OMO_StraightPathHazard::OMO_StraightPathHazard(gfxSprite *nspr, short x, short y, float angle, float vel, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    IO_OverMapObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY, iAnimationOffsetX, iAnimationOffsetY, iAnimationHeight, iAnimationWidth)
{
    objectType = object_pathhazard;

    dVel = vel;
    dAngle = angle;

    velx = vel * cos(angle);
    vely = vel * sin(angle);
}

void OMO_StraightPathHazard::update()
{
    IO_OverMapObject::update();

    if (iy + ih < 0 || iy >= App::screenHeight)
        dead = true;

    //Wrap hazard if it is off the edge of the screen
    if (ix < 0)
        ix += App::screenWidth;
    else if (ix + iw >= App::screenWidth)
        ix -= App::screenWidth;
}

bool OMO_StraightPathHazard::collide(CPlayer * player)
{
    if (!player->isShielded()) {
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 2) - 16, iy + (ih >> 2) - 16, 3, 8));
        dead = true;

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
        }
    }

    return false;
}
