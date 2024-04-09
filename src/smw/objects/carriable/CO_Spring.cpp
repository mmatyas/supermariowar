#include "CO_Spring.h"

#include "GameValues.h"
#include "map.h"
#include "player.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class spring
//------------------------------------------------------------------------------
CO_Spring::CO_Spring(gfxSprite *nspr, short iX, short iY, bool fsuper) :
    MO_CarriedObject(nspr, iX, iY, 4, 4, 30, 31, 1, 0)
{
    fSuper = fsuper;
    iOffsetY = fSuper ? 32 : 0;

    state = 1;
    movingObjectType = movingobject_carried;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;
}

bool CO_Spring::collide(CPlayer * player)
{
    if (owner == NULL) {
        if (player->fOldY + PH <= fOldY && player->iy + PH >= iy)
            hittop(player);
        else if (state == 1)
            hitother(player);
    }

    return false;
}

void CO_Spring::hittop(CPlayer * player)
{
    state = 2;
    drawframe += iw;

    player->setYi(iy - PH - 1);
    player->collisions.checktop(*player);
    player->platform = NULL;
    player->inair = false;
    player->fallthrough = false;
    player->killsinrowinair = 0;
    player->extrajumps = 0;

    player->superjumptimer = 4;
    player->superjumptype = fSuper ? 2 : 1;
    player->vely = -VELNOTEBLOCKREPEL;

    ifSoundOnPlay(rm->sfx_bump);
}

void CO_Spring::hitother(CPlayer * player)
{
    if (owner == NULL && player->isready()) {
        if (player->AcceptItem(this)) {
            owner = player;
        }
    }
}

void CO_Spring::update()
{
    if (owner) {
        MoveToOwner();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    if (state == 2) {
        if (++animationtimer == animationspeed) {
            animationtimer = 0;

            drawframe += iw;
            if (drawframe >= animationWidth) {
                drawframe = 0;
                state = 1;
            }
        }
    }
}

void CO_Spring::draw()
{
    if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX, iOffsetY, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX, iOffsetY, 32, 32);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, animationOffsetX + drawframe, iOffsetY, 32, 32);
    }
}

void CO_Spring::place()
{
    short iAttempts = 10;
    while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
    fx = (float)ix;
    fy = (float)iy;

    Drop();
}


