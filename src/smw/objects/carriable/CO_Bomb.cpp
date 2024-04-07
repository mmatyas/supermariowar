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
// class bomb
//------------------------------------------------------------------------------
CO_Bomb::CO_Bomb(gfxSprite *nspr, short x, short y, float fVelX, float fVelY, short aniSpeed, short iGlobalID, short teamID, short iColorID, short timetolive) :
    MO_CarriedObject(nspr, x, y, 5, aniSpeed, 24, 24, 4, 13)
{
    iw = 28;
    ih = 38;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    iColorOffsetY = (iColorID + 1) * 38;

    movingObjectType = movingobject_bomb;
    state = 1;

    ttl = timetolive;

    velx = fVelX;
    vely = fVelY;

    iOwnerRightOffset = 14;
    iOwnerLeftOffset = -16;
    iOwnerUpOffset = 40;

    sSpotlight = NULL;
}

bool CO_Bomb::collide(CPlayer * player)
{
    if (state == 1 && owner == NULL) {
        if (player->AcceptItem(this)) {
            owner = player;

            velx = 0.0f;
            vely = 0.0f;

            if (iPlayerID == -1) {
                iPlayerID = owner->globalID;
                iTeamID = owner->teamID;
                iColorOffsetY = (owner->colorID + 1) * 38;
            }
        }
    }

    return false;
}

void CO_Bomb::update()
{
    if (--ttl <= 0)
        Die();

    if (dead)
        return;

    if (owner) {
        MoveToOwner();
    } else {
        applyfriction();

        //Collision detect map
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();

    if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

void CO_Bomb::draw()
{
    if (owner && owner->iswarping())
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih, owner->GetWarpState(), owner->GetWarpPlane());
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, iw, ih);
}

void CO_Bomb::Die()
{
    if (owner) {
        owner->carriedItem = NULL;
        owner = NULL;
    }

	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

	if (player != NULL)
		player->decreaseProjectilesCount();

    dead = true;
    objectcontainer[2].add(new MO_Explosion(&rm->spr_explosion, ix + (iw >> 2) - 96, iy + (ih >> 2) - 64, 2, 4, iPlayerID, iTeamID, KillStyle::Bomb));
    ifSoundOnPlay(rm->sfx_bobombsound);
}

