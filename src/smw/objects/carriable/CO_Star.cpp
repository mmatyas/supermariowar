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
// class star (for star mode)
//------------------------------------------------------------------------------
CO_Star::CO_Star(gfxSprite *nspr, short type, short id) :
    MO_CarriedObject(nspr, 0, 0, 8, 8, 30, 30, 1, 1)
{
    iID = id;

    state = 1;
    iw = 32;
    ih = 32;
    movingObjectType = movingobject_star;

    iType = type;

    iOffsetY = type == 1 ? 32 : 0;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    dKickX = 3.0f;
    dKickY = 6.0f;

    fCarriedByKuriboShoe = true;

    placeStar();

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -22;
    iOwnerUpOffset = 32;
}

bool CO_Star::collide(CPlayer * player)
{
    if (game_values.gamemode->gamemode != game_mode_star)
        return false;

    CGM_Star * starmode = (CGM_Star *)game_values.gamemode;

    timer = 0;
    if (owner == NULL && player->isready()) {
        if (player->throw_star == 0 && player->AcceptItem(this)) {
            owner = player;
        }
    }

    if ((iType == 0 && player->isInvincible()) || player->isShielded() || starmode->isplayerstar(player) || game_values.gamemode->gameover)
        return false;

    CPlayer * oldstar = starmode->swapplayer(iID, player);

    if (owner == oldstar) {
        oldstar->throw_star = 30;
        Kick();
    }

    return false;
}

void CO_Star::update()
{
    if (owner) {
        MoveToOwner();
        timer = 0;
    } else if (++timer > 300) {
        placeStar();
    } else {
        applyfriction();

        //Collision detect map
        IO_MovingObject::update();
    }

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void CO_Star::draw()
{
    if (owner) {
        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);
    } else {
        if (velx != 0.0f)
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, iOffsetY, iw, ih);  //keep the star still while it's moving
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iOffsetY, iw, ih);
    }

    if (owner && owner->iswarping())
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, iType ? 0 : 32, 32, 32, owner->GetWarpState(), owner->GetWarpPlane());
    else
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, iType ? 0 : 32, 32, 32);
}

void CO_Star::placeStar()
{
    if (game_values.gamemode->gamemode != game_mode_star)
        return;

    CGM_Star * starmode = (CGM_Star*)game_values.gamemode;

    timer = 0;

    CPlayer * star = starmode->getstarplayer(iID);

    if (star) {
        setXf(star->fx + HALFPW - 16.0f);
        setYf(star->fy + HALFPH - 17.0f);

        velx = star->velx;
        vely = star->vely;
    }

    Drop();
}


