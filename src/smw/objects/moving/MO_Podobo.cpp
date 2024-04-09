#include "MO_Podobo.h"

#include "GameValues.h"
#include "objecthazard.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_ThrowBlock.h"

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class podobo (for survival mode)
//------------------------------------------------------------------------------
MO_Podobo::MO_Podobo(gfxSprite* nspr, short x, short y, float dVelY, short playerid, short teamid, short colorid, bool isSpawned)
    : IO_MovingObject(nspr, x, y, 4, 6)
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
    // Special slow podobo gravity
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

bool MO_Podobo::collide(CPlayer* player)
{
    if (player->globalID != iPlayerID && (game_values.teamcollision == TeamCollisionStyle::On || iTeamID != player->teamID) && !player->isInvincible() && !player->isShielded() && !player->shyguy) {
        // Find the player that made this explosion so we can attribute a kill
        PlayerKilledPlayer(iPlayerID, player, death_style_jump, KillStyle::Podobo, false, false);
        return true;
    }

    return false;
}

void MO_Podobo::collide(IO_MovingObject* object)
{
    if (iPlayerID == -1)
        return;

    MovingObjectType type = object->getMovingObjectType();

    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_bulletbill) {
        // Same team bullet bills don't kill each other
        if (type == movingobject_bulletbill && ((MO_BulletBill*)object)->iTeamID == iTeamID)
            return;

        if (type == movingobject_shell)
            ((CO_Shell*)object)->Die();
        else if (type == movingobject_throwblock)
            ((CO_ThrowBlock*)object)->Die();
        else if (type == movingobject_bulletbill)
            ((MO_BulletBill*)object)->Die();

        ifSoundOnPlay(rm->sfx_kicksound);
    }
}
