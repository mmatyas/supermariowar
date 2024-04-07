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
// class podobo (for survival mode)
//------------------------------------------------------------------------------
MO_Podobo::MO_Podobo(gfxSprite *nspr, short x, short y, float dVelY, short playerid, short teamid, short colorid, bool isSpawned) :
    IO_MovingObject(nspr, x, y, 4, 6)
{
    fIsSpawned = isSpawned;
    iHiddenPlane = y;

    objectType = object_moving;
    movingObjectType = movingobject_podobo;
    vely = dVelY;

    ih = 32;
    collisionHeight = ih;

    iPlayerID = playerid;
    iTeamID = teamid;
    iColorOffsetY = (colorid + 1) * 64;

    fObjectCollidesWithMap = false;
}

void MO_Podobo::update()
{
    //Special slow podobo gravity
    vely += 0.2f;

    setXf(fx + velx);
    setYf(fy + vely);

    animate();

	if (iy > App::screenHeight - 1 && vely > 0.0f)
        dead = true;
}

void MO_Podobo::draw()
{
    if (fIsSpawned && vely < 0.0f)
        spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih, 2, iHiddenPlane);
    else
        spr->draw(ix, iy, drawframe, iColorOffsetY + (vely > 0.0f ? 32 : 0), iw, ih);
}

bool MO_Podobo::collide(CPlayer * player)
{
    if (player->globalID != iPlayerID && (game_values.teamcollision == TeamCollisionStyle::On|| iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded() && !player->shyguy) {
        //Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Podobo, false, false);
        return true;
    }

    return false;
}

void MO_Podobo::collide(IO_MovingObject * object)
{
    if (iPlayerID == -1)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill) {
        //Same team bullet bills don't kill each other
        if (type == movingobject_bulletbill && ((MO_BulletBill*) object)->iTeamID == iTeamID)
            return;

        if (type == movingobject_shell)
            ((CO_Shell*)object)->Die();
        else if (type == movingobject_throwblock)
            ((CO_ThrowBlock*) object)->Die();
        else if (type == movingobject_bulletbill)
            ((MO_BulletBill*) object)->Die();

        ifSoundOnPlay(rm->sfx_kicksound);
    }
}

