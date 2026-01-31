#include "MO_SuperFireball.h"

#include "eyecandy.h"
#include "Game.h"
#include "GameValues.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

extern CEyecandyContainer eyecandy[3];
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class super fireball
//------------------------------------------------------------------------------
MO_SuperFireball::MO_SuperFireball(gfxSprite* nspr, Vec2s pos, short iNumSpr, Vec2f vel, short aniSpeed, short iGlobalID, short teamID, short iColorID)
    : IO_MovingObject(nspr, pos, iNumSpr, aniSpeed, nspr->getWidth() / iNumSpr, nspr->getHeight() / 10, 0, 0)
{
    ih /= 10;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    colorOffset = (iColorID + 1) * 64;
    directionOffset = velx < 0.0f ? 0 : 32;
    movingObjectType = movingobject_superfireball;

    state = 1;

    velx = vel.x;
    vely = vel.y;
    ttl = RANDOM_INT(30) + 60;

    drawframe = 0;

    fObjectCollidesWithMap = false;
}

void MO_SuperFireball::update()
{
    animate();

    setXf(fx + velx);
    setYf(fy + vely);

    if (ix < 0) {
        setXi(ix + App::screenWidth);
    } else if (ix > App::screenWidth - 1) {
        setXi(ix - App::screenWidth);
    }

    if (iy > App::screenHeight || iy < -ih || --ttl <= 0)
        removeifprojectile(this, false, true);
}

bool MO_SuperFireball::collide(CPlayer* player)
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

void MO_SuperFireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + directionOffset, iw, ih);
}
