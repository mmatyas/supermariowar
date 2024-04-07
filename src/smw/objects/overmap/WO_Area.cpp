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
// class area (for Domination mode)
//------------------------------------------------------------------------------
OMO_Area::OMO_Area(gfxSprite *nspr, short iNumAreas) :
    IO_OverMapObject(nspr, 1280, 960, 5, 0)
{
    iw = (short)spr->getWidth() / 5;
    collisionWidth = iw;

    objectType = object_area;
    iPlayerID = -1;
    iTeamID = -1;

    colorID = -1;
    scoretimer = 0;

    state = 1;

    frame = 0;
    relocatetimer = 0;
    numareas = iNumAreas;
    totalTouchingPlayers = 0;
    touchingPlayer = NULL;

    placeArea();
}

bool OMO_Area::collide(CPlayer * player)
{
    if (player->tanookisuit.notStatue() && !player->isdead()) {
        totalTouchingPlayers++;

        if (totalTouchingPlayers == 1) {
            touchingPlayer = player;
        } else {
            touchingPlayer = NULL;
            reset();
        }
    }

    return false;
}

void OMO_Area::draw()
{
    spr->draw(ix, iy, frame, 0, iw, ih);
}

void OMO_Area::update()
{
    if (touchingPlayer)
        setOwner(touchingPlayer);

    if (iPlayerID != -1 && !game_values.gamemode->gameover) {
        if (++scoretimer >= (game_values.pointspeed << 1)) {
            scoretimer = 0;
            list_players[iPlayerID]->Score().AdjustScore(1);
            game_values.gamemode->CheckWinner(list_players[iPlayerID]);
        }
    }

    if (game_values.gamemodesettings.domination.relocationfrequency > 0) {
        if (++relocatetimer >= game_values.gamemodesettings.domination.relocationfrequency) {
            relocatetimer = 0;
            placeArea();
        }
    }

    totalTouchingPlayers = 0;
    touchingPlayer = NULL;
}

void OMO_Area::placeArea()
{
    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[0].getClosestObject(x, y, object_area) <= (200.0f - ((numareas - 3) * 25.0f)))
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}

void OMO_Area::reset()
{
    iPlayerID = -1;
    iTeamID = -1;

    colorID = -1;
    scoretimer = 0;
    frame = 0;
}

void OMO_Area::setOwner(CPlayer * player)
{
    if (colorID != player->colorID) {
        iPlayerID = player->localID;
        iTeamID = player->teamID;
        colorID = player->colorID;

        frame = (colorID + 1) * iw;
        ifSoundOnPlay(rm->sfx_areatag);
    }
}

