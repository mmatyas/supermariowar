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
// class Ice Blast
//------------------------------------------------------------------------------
MO_IceBlast::MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, 4, 8, 32, 32, 0, 0, 0, (iColorID + 1) << 5, 32, 32)
{
    iPlayerID = iGlobalID;
    iTeamID = teamID;

    movingObjectType = movingobject_iceblast;

    state = 1;

    velx = fVelyX;
    vely = 0.0f;

    if (velx > 0.0f)
        drawframe = 0;
    else
        drawframe = animationWidth - iw;

    ttl = 120;

    fObjectCollidesWithMap = false;

    sSpotlight = NULL;
}

void MO_IceBlast::update()
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

    if (--ttl <= 0) {
        removeifprojectile(this, false, true);
    } else if (game_values.spotlights) {
        if (!sSpotlight) {
            sSpotlight = spotlightManager.AddSpotlight(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1), 3);
        }

        if (sSpotlight) {
            sSpotlight->UpdatePosition(ix - collisionOffsetX + (iw >> 1), iy - collisionOffsetY + (ih >> 1));
        }
    }
}

bool MO_IceBlast::collide(CPlayer * player)
{
    if (iPlayerID != player->globalID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID)) {
        if (!player->isShielded() && !player->isInvincible() && !player->shyguy) {
            player->makefrozen(game_values.wandfreezetime);
            removeifprojectile(this, false, true);
        }
    }

    return false;
}

void MO_IceBlast::draw()
{
    IO_MovingObject::draw();
}

