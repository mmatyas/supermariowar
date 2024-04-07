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
// class flag (for Capture the Flag mode)
//------------------------------------------------------------------------------
CO_Flag::CO_Flag(gfxSprite *nspr, MO_FlagBase * base, short iTeamID, short iColorID) :
    MO_CarriedObject(nspr, 0, 0, 4, 8, 30, 30, 1, 1, 0, iColorID << 6, 32, 32)
{
    state = 1;
    movingObjectType = movingobject_flag;
    flagbase = base;
    teamID = iTeamID;
    fLastFlagDirection = false;
    owner_throw = NULL;
    owner_throw_timer = 0;

    centerflag = teamID == -1;

    iOwnerRightOffset = HALFPW - 31;
    iOwnerLeftOffset = HALFPW + 1;
    iOwnerUpOffset = 38;

    fCarriedByKuriboShoe = true;

    placeFlag();
}

bool CO_Flag::collide(CPlayer * player)
{
    if (owner == NULL && player->isready() && (!fInBase || teamID != player->teamID)) {
        if (game_values.gamemodesettings.flag.touchreturn && teamID == player->teamID) {
            placeFlag();
            ifSoundOnPlay(rm->sfx_areatag);
        } else if (player->AcceptItem(this)) {
            owner = player;
            owner_throw = player;

            if (flagbase)
                flagbase->setFlag(NULL);
        }
    }

    return false;
}

void CO_Flag::update()
{
    if (owner) {
        MoveToOwner();
        timer = 0;
        fInBase = false;
        owner_throw = owner;
    } else if (fInBase) {
        if (flagbase) {
            setXf(flagbase->fx);
            setYf(flagbase->fy);
        }

        owner_throw = NULL;
    } else if (game_values.gamemodesettings.flag.autoreturn > 0 && ++timer > game_values.gamemodesettings.flag.autoreturn) {
        timer = 0;
        placeFlag();
        owner_throw = NULL;
    } else {
        if (owner_throw && --owner_throw_timer < 0) {
            owner_throw_timer = 0;
            owner_throw = NULL;
        }

        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (centerflag) {
        if (animationtimer % 2 == 0) {
            animationOffsetY += 64;
            if (animationOffsetY > 192)
                animationOffsetY = 0;
        }
    }
}

void CO_Flag::draw()
{
    if (owner) {
        if (centerflag)
            animationOffsetY = owner->colorID << 6;

        if (owner->iswarping())
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
        else
            spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY + (fLastFlagDirection ? 32 : 0), iw, ih);
    }
}

void CO_Flag::MoveToOwner()
{
    MO_CarriedObject::MoveToOwner();

    if (owner)
        fLastFlagDirection = owner->isFacingRight();
}

void CO_Flag::placeFlag()
{
    if (centerflag) {
        Drop();
        fInBase = false;

        short iAttempts = 10;
        while (!g_map->findspawnpoint(5, &ix, &iy, collisionWidth, collisionHeight, false) && iAttempts-- > 0);
        fx = (float)ix;
        fy = (float)iy;

        velx = 0.0f;
        vely = 0.0f;
        fLastFlagDirection = false;
    } else if (flagbase) {
        Drop();
        fInBase = true;
        setXi(flagbase->ix);
        setYi(flagbase->iy);
        fLastFlagDirection = false;
        flagbase->setFlag(this);
    }

    owner_throw = NULL;
    owner_throw_timer = 0;

    timer = 0;
}

void CO_Flag::Drop()
{
    MO_CarriedObject::Drop();
    owner_throw_timer = 62;
}

