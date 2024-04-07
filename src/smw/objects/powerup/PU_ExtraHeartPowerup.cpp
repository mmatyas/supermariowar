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
// class special heart powerup for health mode
//------------------------------------------------------------------------------
PU_ExtraHeartPowerup::PU_ExtraHeartPowerup(gfxSprite *nspr, short x, short y) :
    MO_Powerup(nspr, x, y, 1, 0, 30, 30, 1, 1)
{
    velx = 0.0f;
}

bool PU_ExtraHeartPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        if (game_values.gamemode->gamemode == game_mode_health) {
            if (player->Score().subscore[1] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[1]++;

            if (player->Score().subscore[0] < game_values.gamemodesettings.health.maxlife)
                player->Score().subscore[0]++;
        }

        ifSoundOnPlay(rm->sfx_collectpowerup);
        dead = true;
    }

    return false;
}
