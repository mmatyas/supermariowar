#include "MO_AttackZone.h"

#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_ThrowBlock.h"
#include "objects/carriable/CO_ThrowBox.h"

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// attack zone(invisible area that kills objects and players)
//------------------------------------------------------------------------------
MO_AttackZone::MO_AttackZone(short playerId, short teamId, short x, short y, short w, short h, short time, KillStyle style, bool dieoncollision)
    : IO_MovingObject(NULL, x, y, 1, 0, w, h, 0, 0)
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

bool MO_AttackZone::collide(CPlayer* player)
{
    if (player->isShielded() || player->isInvincible() || player->shyguy || dead)
        return false;

    if (game_values.teamcollision != TeamCollisionStyle::On && player->teamID == iTeamID)
        return false;

    CPlayer* killer = GetPlayerFromGlobalID(iPlayerID);

    if (killer && killer->globalID == player->globalID)
        return false;

    PlayerKilledPlayer(iPlayerID, player, PlayerDeathStyle::Jump, iStyle, false, false);

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

void MO_AttackZone::collide(IO_MovingObject* object)
{
    if (dead)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
        if (type == movingobject_shell) {
            CO_Shell* shell = (CO_Shell*)object;
            if (shell->frozen)
                shell->ShatterDie();
            else
                shell->Die();
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock* block = (CO_ThrowBlock*)object;

            if (block->frozen)
                block->ShatterDie();
            else if (!block->owner || block->owner->globalID != iPlayerID)
                block->Die();
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox* box = (CO_ThrowBox*)object;

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
