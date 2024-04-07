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
// class buzzy beetle
//------------------------------------------------------------------------------
MO_BuzzyBeetle::MO_BuzzyBeetle(gfxSprite *nspr, bool moveToRight) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 20, 1, 11, 0, moveToRight ? 0 : 32, 32, 32, moveToRight, false, false, true)
{
    movingObjectType = movingobject_buzzybeetle;
    iSpawnIconOffset = 160;
    killStyle = KillStyle::BuzzyBeetle;
}

void MO_BuzzyBeetle::update()
{
    if (velx < 0.0f)
        animationOffsetY = 32;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_BuzzyBeetle::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;
    dead = true;

    player->AddKillerAward(NULL, KillStyle::BuzzyBeetle);

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    ifSoundOnPlay(rm->sfx_mip);

    DropShell(false, false);

    return false;
}

void MO_BuzzyBeetle::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, 96, 0, 32, 32));
}

void MO_BuzzyBeetle::DropShell(bool fBounce, bool fFlip)
{
    //Give the shell a state 2 so it is already spawned but sitting
    CO_Shell * shell = new CO_Shell(3, ix - 1, iy, false, true, false, false);
    shell->nospawn(iy, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}

