#include "objectgame.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gamemodes/BonusHouse.h"
#include "gamemodes/Chase.h"
#include "gamemodes/Race.h"
#include "gamemodes/Star.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/MiniBoxes.h"
#include "gamemodes/MiniPipe.h"

#include <cstdlib> // abs()
#include <cmath>

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern void PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);
extern void CheckSecret(short id);
extern SpotlightManager spotlightManager;
extern CObjectContainer objectcontainer[3];

extern CGM_Pipe_MiniGame * pipegamemode;

extern CPlayer* list_players[4];
extern short score_cnt;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
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


