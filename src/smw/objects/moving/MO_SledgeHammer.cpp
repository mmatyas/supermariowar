#include "MO_SledgeHammer.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/moving/MO_Explosion.h"
#include "objects/moving/MO_Hammer.h"

#include <cmath>

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CObjectContainer objectcontainer[3];
extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class sledge hammer
//------------------------------------------------------------------------------
MO_SledgeHammer::MO_SledgeHammer(gfxSprite* nspr, Vec2s pos, short iNumSpr, Vec2f vel, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer)
    : IO_MovingObject(nspr, pos, iNumSpr, aniSpeed, (short)nspr->getWidth() / iNumSpr, (short)nspr->getHeight() / 5, 0, 0)
{
    ih = collisionHeight;

    playerID = iGlobalID;
    teamID = iTeamID;
    colorOffset = (iColorID + 1) * 32;
    movingObjectType = movingobject_sledgehammer;
    state = 1;
    velx = vel.x;
    vely = vel.y;

    fSuper = superHammer;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;
}

void MO_SledgeHammer::update()
{
    if (++animationtimer == animationspeed) {
        animationtimer = 0;

        if (velx > 0) {
            drawframe += iw;
            if (drawframe >= animationWidth)
                drawframe = 0;
        } else {
            drawframe -= iw;
            if (drawframe < 0)
                drawframe = animationWidth - iw;
        }
    }

    setXf(fx + velx);
    setYf(fy + vely);

    vely += GRAVITATION;

    if (ix < 0)
        setXi(ix + App::screenWidth);
    else if (ix > App::screenWidth - 1)
        setXi(ix - App::screenWidth);

    if (iy >= App::screenHeight)
        dead = true;
}

bool MO_SledgeHammer::collide(CPlayer* player)
{
    if (!player->isShielded()) {
        dead = true;
        eyecandy[2].emplace<EC_SingleAnimation>(&rm->spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4);
        ifSoundOnPlay(rm->sfx_hit);

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
        }
    }

    return false;
}

void MO_SledgeHammer::explode()
{
    if (fSuper) {
        objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, {ix + (iw >> 2) - 96, iy + (ih >> 2) - 64}, 2, 4, -1, -1, KillStyle::Hammer));
        ifSoundOnPlay(rm->sfx_bobombsound);
    } else {
        short iCenterX = ix + (iw >> 1) - 14;
        short iCenterY = iy + (ih >> 1) - 14;
        short iColorID = colorOffset / 32 - 1;

        for (short iHammer = 0; iHammer < 3; iHammer++) {
            float dAngle = (float)(RANDOM_INT(628)) / 100.0f;
            float dVel = (float)(RANDOM_INT(5)) / 2.0f + 3.0f;
            float dVelX = dVel * cos(dAngle);
            float dVelY = dVel * sin(dAngle);
            objectcontainer[2].add(new MO_Hammer(&rm->spr_hammer, {iCenterX, iCenterY}, 6, {dVelX, dVelY}, 5, playerID, teamID, iColorID, true));
        }

        CPlayer* player = GetPlayerFromGlobalID(playerID);

        player->increaseProjectilesCount(3);

        ifSoundOnPlay(rm->sfx_cannon);
    }
}

void MO_SledgeHammer::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset, iw, ih);
}
