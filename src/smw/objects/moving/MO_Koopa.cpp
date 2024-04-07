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
// class koopa
//------------------------------------------------------------------------------
MO_Koopa::MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing, bool bFallOffLedges) :
    MO_WalkingEnemy(nspr, 2, 8, 30, 28, 1, 25, 0, moveToRight ? 0 : 54, 54, 32, moveToRight, true, fBouncing, bFallOffLedges)
{
    fRed = red;
    movingObjectType = movingobject_koopa;
    iSpawnIconOffset = fRed ? 144 : 112;
    killStyle = KillStyle::Koopa;
}

void MO_Koopa::draw()
{
    //if frozen, just draw shell, not entire koopa
    if (frozen) {
        rm->spr_shell.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, fRed ? 32 : 0, 32, 32);
        rm->spr_iceblock.draw(ix - collisionOffsetX + iw - 32, iy - collisionOffsetY + ih - 32, 0, 0, 32, 32);
    } else {
        MO_WalkingEnemy::draw();
    }
}

void MO_Koopa::update()
{
    if (velx < 0.0f)
        animationOffsetY = 54;
    else
        animationOffsetY = 0;

    MO_WalkingEnemy::update();
}

bool MO_Koopa::hittop(CPlayer * player)
{
    player->setYi(iy - PH - 1);
    player->bouncejump();
    player->collisions.checktop(*player);
    player->platform = NULL;

    if (game_values.gamemode->gamemode == game_mode_stomp && !game_values.gamemode->gameover)
        player->Score().AdjustScore(1);

    if (fBouncing) {
        fBouncing = false;
        bounce = GRAVITATION;

        if (vely < GRAVITATION)
            vely = GRAVITATION;

        spr = fRed ? &rm->spr_redkoopa : &rm->spr_koopa;
    } else {
        dead = true;

        player->AddKillerAward(NULL, KillStyle::Koopa);

        DropShell(false, false);
    }

    ifSoundOnPlay(rm->sfx_mip);

    return false;
}

void MO_Koopa::Die()
{
    if (frozen) {
        ShatterDie();
        return;
    }

    dead = true;
    eyecandy[2].add(new EC_FallingObject(&rm->spr_shelldead, ix, iy, 0.0f, -VELJUMP / 2.0f, 1, 0, fRed ? 32 : 0, 0, 32, 32));
}

void MO_Koopa::DropShell(bool fBounce, bool fFlip)
{
    //Give the shell a state 2 so it is already spawned but sitting
    CO_Shell * shell;

    if (fRed)
        shell = new CO_Shell(1, ix - 1, iy + 8, false, true, true, false);
    else
        shell = new CO_Shell(0, ix - 1, iy + 8, true, true, true, false);

    shell->nospawn(iy + 8, fBounce);

    if (fFlip)
        shell->Flip();

    objectcontainer[1].add(shell);
}

