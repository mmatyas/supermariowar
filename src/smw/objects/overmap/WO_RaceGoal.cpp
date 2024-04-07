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
// class race goal (for Race mode)
//------------------------------------------------------------------------------
short flagpositions[3][4][2] = { { {18, 20}, {34, 20}, {0, 0}, {0, 0} },
    { {18, 20}, {34, 20}, {26, 36}, {0, 0} },
    { {18, 20}, {34, 20}, {18, 36}, {34, 36} }
};

OMO_RaceGoal::OMO_RaceGoal(gfxSprite *nspr, short id) :
    IO_OverMapObject(nspr, 0, 0, 2, 8)
{
    iw = (short)spr->getWidth() >> 1;
    ih = (short)spr->getHeight() >> 1;
    collisionWidth = 36;
    collisionHeight = 36;
    collisionOffsetX = 16;
    collisionOffsetY = 18;

    objectType = object_race_goal;
    state = 1;

    goalID = id;

    for (short k = 0; k < 4; k++)
        tagged[k] = -1;

    angle = (float)(RANDOM_INT(1000) * 0.00628f);
    anglechange = (float)(RANDOM_INT(100) * 0.0002f);
    anglechangetimer = (short)(RANDOM_INT(50) + 100);

    velx = sin(angle);
    vely = cos(angle);

    placeRaceGoal();

    speed = (float)game_values.gamemodesettings.race.speed / 4.0f;
    quantity = game_values.gamemodesettings.race.quantity;
    isfinishline = goalID == quantity - 1;
}

bool OMO_RaceGoal::collide(CPlayer * player)
{
    if (game_values.gamemode->getgamemode() == game_mode_race && player->tanookisuit.notStatue()) {
        CGM_Race * gamemode = (CGM_Race*)game_values.gamemode;

        if (tagged[player->teamID] != player->colorID && gamemode->getNextGoal(player->teamID) >= goalID) {
            tagged[player->teamID] = player->colorID;

            if (isfinishline)
                ifSoundOnPlay(rm->sfx_racesound);
            else
                ifSoundOnPlay(rm->sfx_areatag);
        }

        if (gamemode->getNextGoal(player->teamID) == goalID)
            gamemode->setNextGoal(player->teamID);
    }
    return false;
}

void OMO_RaceGoal::draw()
{
    if (isfinishline) {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 54, iw, ih);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);

        for (short k = 0; k < score_cnt; k++) {
            if (tagged[k] > -1)
                rm->spr_bonus.draw(ix - collisionOffsetX + flagpositions[score_cnt - 2][k][0], iy - collisionOffsetY + flagpositions[score_cnt - 2][k][1], 0, tagged[k] * 16, 16, 16);
        }
    }

    rm->spr_racetext.draw(ix - collisionOffsetX + 26, iy - collisionOffsetY, (goalID + 1) * 16, 0, 16, 16);
}

void OMO_RaceGoal::update()
{
    if (--anglechangetimer <= 0) {
        anglechange = (float)((RANDOM_INT(101) - 50) * 0.0002f);
        anglechangetimer = (short)(RANDOM_INT(50) + 100);
    }

    angle += anglechange;

    velx = speed * sin(angle);
    vely = speed * cos(angle);

    IO_OverMapObject::update();

    ix = (short)fx;
    iy = (short)fy;

    if (ix < 0) {
        velx = -velx;
        ix = 0;
        fx = (float)ix;

        angle = atan2(velx, vely);
    } else if (ix + collisionWidth >= App::screenWidth) {
        velx = -velx;
        ix = App::screenWidth - 1 - collisionWidth;
        fx = (float)ix;

        angle = atan2(velx, vely);
    }

    if (iy < 0) {
        vely = -vely;
        iy = 0;
        fy = (float)iy;

        angle = atan2(velx, vely);
	} else if (iy + collisionHeight >= App::screenHeight) {
        vely = -vely;
		iy = App::screenHeight - 1 - collisionHeight;
        fy = (float)iy;

        angle = atan2(velx, vely);
    }
}

void OMO_RaceGoal::placeRaceGoal()
{
    short x = 0, y = 0;

    if (goalID < g_map->iNumRaceGoals) {
        x = g_map->racegoallocations[goalID].x;
        y = g_map->racegoallocations[goalID].y;
    } else {
        short tries = 0;
        do {
            if (++tries > 32)
                break;

            x = (short)RANDOM_INT(App::screenWidth - collisionWidth);
            y = (short)RANDOM_INT(App::screenHeight - collisionHeight);
        } while (objectcontainer[2].getClosestObject(x, y, object_race_goal) <= 250.0f - (quantity * 25.0f));
    }

    setXi(x);
    setYi(y);
}

