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
// class special extra coin powerup for coin or greed mode
//------------------------------------------------------------------------------
PU_CoinPowerup::PU_CoinPowerup(gfxSprite *nspr, short x, short y, short color, short value) :
    MO_Powerup(nspr, x, y, 4, 8, 30, 30, 1, 1)
{
    velx = 0.0f;
    iColorOffsetY = color << 5;
    iValue = value;

    sparkleanimationtimer = 0;
    sparkledrawframe = 0;
}

void PU_CoinPowerup::update()
{
    MO_Powerup::update();

    if (++sparkleanimationtimer >= 4) {
        sparkleanimationtimer = 0;
        sparkledrawframe += 32;
        if (sparkledrawframe >= App::screenHeight)
            sparkledrawframe = 0;
    }
}

void PU_CoinPowerup::draw()
{
    if (state == 0) {
        short iHeight = (short)(32 - fy + desty);
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, iHeight);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, iHeight);
    } else {
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, iColorOffsetY, 32, 32);
        rm->spr_shinesparkle.draw(ix - collisionOffsetX, iy - collisionOffsetY, sparkledrawframe, 0, 32, 32);
    }
}

bool PU_CoinPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_coins || game_values.gamemode->gamemode == game_mode_greed) {
            if (!game_values.gamemode->gameover) {
                player->Score().AdjustScore(iValue);
                game_values.gamemode->CheckWinner(player);
            }
        }

        ifSoundOnPlay(rm->sfx_coin);
        dead = true;
    }

    return false;
}


