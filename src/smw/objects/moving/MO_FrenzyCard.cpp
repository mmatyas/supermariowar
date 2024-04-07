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
// class frenzycard (for fire frenzy mode)
//------------------------------------------------------------------------------
MO_FrenzyCard::MO_FrenzyCard(gfxSprite *nspr, short iType) :
    IO_MovingObject(nspr, 0, 0, 12, 8, -1, -1, -1, -1, 0, iType * 32, 32, 32)
{
    state = 1;
    objectType = object_frenzycard;
    type = iType;

    if (type == NUMFRENZYCARDS - 1)
        type = RANDOM_INT (NUMFRENZYCARDS - 1);

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;

    placeCard();

    fObjectCollidesWithMap = false;
}

bool MO_FrenzyCard::collide(CPlayer * player)
{
    if (type < 14 || type > 17 || game_values.gamemodesettings.frenzy.storedshells) {
        player->SetPowerup(type);
        game_values.gamemode->frenzyowner = player;
    } else {
        switch (type) {
        case 14: {
            CO_Shell * shell = new CO_Shell(0, 0, 0, true, true, true, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 15: {
            CO_Shell * shell = new CO_Shell(1, 0, 0, false, true, true, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 16: {
            CO_Shell * shell = new CO_Shell(2, 0, 0, false, false, true, true);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        case 17: {
            CO_Shell * shell = new CO_Shell(3, 0, 0, false, true, false, false);
            if (objectcontainer[1].add(shell))
                shell->UsedAsStoredPowerup(player);
            break;
        }
        }
    }

    dead = true;
    return false;
}

void MO_FrenzyCard::update()
{
    animate();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }

    if (++timer > 1500)
        placeCard();
}

void MO_FrenzyCard::draw()
{
    IO_MovingObject::draw();

    //Draw sparkles
    rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
}

void MO_FrenzyCard::placeCard()
{
    timer = 0;

    short x = 0, y = 0;
    short iAttempts = 32;
    while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) ||
            objectcontainer[1].getClosestObject(x, y, object_frenzycard) <= 150.0f)
            && iAttempts-- > 0);

    setXi(x);
    setYi(y);
}

