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
// class Bowser Fire (for survival mode)
//------------------------------------------------------------------------------
OMO_BowserFire::OMO_BowserFire(gfxSprite *nspr, short x, short y, float dVelX, float dVelY, short id, short teamid, short colorid) :
    IO_OverMapObject(nspr, x, y, 3, 6)
{
    objectType = object_bowserfire;
    velx = dVelX;
    vely = dVelY;

    ih = 32;
    collisionHeight = ih;

    iPlayerID = id;
    iTeamID = teamid;
    iColorOffsetY = (colorid + 1) * 64;
}

void OMO_BowserFire::update()
{
    IO_OverMapObject::update();

    if ((velx < 0 && ix < -iw) || (velx > 0 && ix > App::screenWidth)) {
    	CPlayer * player = GetPlayerFromGlobalID(iPlayerID);

    	if (player != NULL)
    		player->decreaseProjectilesCount();

        dead = true;
    }
}

void OMO_BowserFire::draw()
{
    spr->draw(ix, iy, drawframe, (velx > 0.0f ? 32 : 0) + iColorOffsetY, iw, ih);
}

bool OMO_BowserFire::collide(CPlayer * player)
{
    //if the fire is off the screen, don't wrap it to collide
    if ((ix < 0 && velx < 0.0f && player->ix > ix + iw && player->ix + PW < App::screenWidth) ||
            (ix + iw >= App::screenWidth && velx > 0.0f && player->ix + PW < ix && player->ix >= 0)) {
        return false;
    }

    if (player->globalID != iPlayerID && (game_values.teamcollision == TeamCollisionStyle::On|| iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded()) {
        //Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Fireball, false, false);
        return true;
    }

    return false;
}


