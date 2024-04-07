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
// class super fireball
//------------------------------------------------------------------------------
MO_SuperFireball::MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short teamID, short iColorID) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, nspr->getWidth() / iNumSpr, nspr->getHeight() / 10, 0, 0)
{
    ih /= 10;

    iPlayerID = iGlobalID;
    iTeamID = teamID;
    colorOffset = (iColorID + 1) * 64;
    directionOffset = velx < 0.0f ? 0 : 32;
    movingObjectType = movingobject_superfireball;

    state = 1;

    velx = fVelyX;
    vely = fVelyY;
    ttl = RANDOM_INT(30) + 60;

    drawframe = 0;

    fObjectCollidesWithMap = false;
}

void MO_SuperFireball::update()
{
    animate();

    setXf(fx + velx);
    setYf(fy + vely);

    if (ix < 0) {
        setXi(ix + App::screenWidth);
    } else if (ix > App::screenWidth - 1) {
        setXi(ix - App::screenWidth);
    }

	if (iy > App::screenHeight|| iy < -ih || --ttl <= 0)
        removeifprojectile(this, false, true);
}

bool MO_SuperFireball::collide(CPlayer * player)
{
    if (!player->isShielded()) {
        dead = true;
        eyecandy[2].add(new EC_SingleAnimation(&rm->spr_fireballexplosion, ix + (iw >> 1) - 16, iy + (ih >> 1) - 16, 3, 4));
        ifSoundOnPlay(rm->sfx_hit);

        if (!player->isInvincible() && !player->shyguy) {
            return player->KillPlayerMapHazard(false, KillStyle::Environment, false) != PlayerKillType::NonKill;
        }
    }

    return false;
}

void MO_SuperFireball::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, colorOffset + directionOffset, iw, ih);
}

