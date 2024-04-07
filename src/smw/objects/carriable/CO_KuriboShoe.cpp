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
// class kuribo's shoe
//------------------------------------------------------------------------------
CO_KuriboShoe::CO_KuriboShoe(gfxSprite *nspr, short iX, short iY, bool sticky) :
    CO_Spring(nspr, iX, iY + 15, false)
{
    iw = 32;
    ih = 32;

    collisionOffsetY = 15;
    collisionHeight = 16;

    animationOffsetX = sticky ? 64 : 0;

    movingObjectType = movingobject_carried;

    fSticky = sticky;
}

void CO_KuriboShoe::hittop(CPlayer * player)
{
    if (!player->kuriboshoe.is_on() && player->tanookisuit.notStatue()) {
        dead = true;
        player->SetKuriboShoe(fSticky ? STICKY : NORMAL);
        ifSoundOnPlay(rm->sfx_transform);
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, player->ix + HALFPW - 16, player->iy + HALFPH - 16, 3, 8));
    }
}


