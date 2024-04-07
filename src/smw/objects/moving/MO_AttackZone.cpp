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
// attack zone(invisible area that kills objects and players)
//------------------------------------------------------------------------------
MO_AttackZone::MO_AttackZone(short playerId, short teamId, short x, short y, short w, short h, short time, KillStyle style, bool dieoncollision) :
    IO_MovingObject(NULL, x, y, 1, 0, w, h, 0, 0)
{
    iPlayerID = playerId;
    iTeamID = teamId;
    iStyle = style;

    objectType = object_moving;
    movingObjectType = movingobject_attackzone;

    iTimer = time;
    fDieOnCollision = dieoncollision;

    state = 1;

    fObjectCollidesWithMap = false;
}

bool MO_AttackZone::collide(CPlayer * player)
{
    if (player->isShielded() || player->isInvincible() || player->shyguy || dead)
        return false;

    if (game_values.teamcollision != TeamCollisionStyle::On && player->teamID == iTeamID)
        return false;

    CPlayer * killer = GetPlayerFromGlobalID(iPlayerID);

    if (killer && killer->globalID == player->globalID)
        return false;

    PlayerKilledPlayer(iPlayerID, player, death_style_jump, iStyle, false, false);

    Die();

    return true;
}

/*void MO_AttackZone::draw()
{
	if (!dead)
	{
		SDL_Rect r = {ix, iy, collisionWidth, collisionHeight};
		SDL_FillRect(blitdest, &r, 0xf000);
	}
}*/

void MO_AttackZone::update()
{
    if (--iTimer <= 0)
        dead = true;
}

void MO_AttackZone::collide(IO_MovingObject * object)
{
    if (dead)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
        if (type == movingobject_shell) {
            CO_Shell * shell = (CO_Shell*)object;
            if (shell->frozen)
                shell->ShatterDie();
            else
                shell->Die();
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock * block = (CO_ThrowBlock*) object;

            if (block->frozen)
                block->ShatterDie();
            else if (!block->owner || block->owner->globalID != iPlayerID)
                block->Die();
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox * box = (CO_ThrowBox*) object;

            if (box->frozen)
                box->ShatterDie();
            else if (!box->owner || box->owner->globalID != iPlayerID)
                box->Die();
        }

        ifSoundOnPlay(rm->sfx_kicksound);
    }
}

void MO_AttackZone::Die()
{
    if (fDieOnCollision)
        dead = true;
}


