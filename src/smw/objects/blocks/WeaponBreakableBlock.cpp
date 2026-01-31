#include "WeaponBreakableBlock.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "GlobalConstants.h"
#include "map.h"
#include "player.h"
#include "PlayerKillTypes.h"
#include "ResourceManager.h"

#include "objects/moving/MovingObject.h"
#include "objects/moving/MO_AttackZone.h"
#include "objects/moving/MO_Fireball.h"
#include "objects/carriable/CO_Shell.h"
#include "objects/carriable/CO_ThrowBlock.h"
#include "objects/carriable/CO_ThrowBox.h"

extern CMap* g_map;
extern CResourceManager* rm;
extern CGameValues game_values;
extern CEyecandyContainer eyecandy[3];

B_WeaponBreakableBlock::B_WeaponBreakableBlock(gfxSprite *nspr, Vec2s pos, WeaponDamageType type)
    : IO_Block(nspr, pos)
    , iType(type)
    , iDrawOffsetX(static_cast<short>(type) * 32)
{
    iw = TILESIZE;
    ih = TILESIZE;
}

void B_WeaponBreakableBlock::draw()
{
    if (state == 0)
        spr->draw(ix, iy, iDrawOffsetX, 0, iw, ih);
}

void B_WeaponBreakableBlock::update()
{
    if (state > 0) {
        if (state == 1) {
            state = 2;
        } else if (state == 2) {
            iBumpPlayerID = -1;
            dead = true;
            g_map->blockdata[col][row] = NULL;
            g_map->UpdateTileGap(col, row);
        }
    }
}


bool B_WeaponBreakableBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);

    if (state == 1 || state == 2) {
        PlayerKillType iKillType = PlayerKillType::NonKill;
        if (iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == TeamCollisionStyle::On))
            PlayerKilledPlayer(iBumpPlayerID, player, PlayerDeathStyle::Jump, KillStyle::Bounce, false, false);

        if (PlayerKillType::NonKill == iKillType)
            player->vely = -VELNOTEBLOCKREPEL;
    } else if (useBehavior) {
        player->vely = GRAVITATION;

        //Save this for when we create a super stomp destroyable block
        if (iType == WeaponDamageType::KuriboShoe && player->IsSuperStomping() && state == 0) {
            triggerBehavior(player->globalID, player->teamID);
            return false;
        } else if (iType == WeaponDamageType::Star && player->isInvincible()) {
            triggerBehavior(player->globalID, player->teamID);
            return false;
        }
    }

    return false;
}

bool B_WeaponBreakableBlock::hitbottom(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        //If the player has a cape and they used it for a feather destroyable block, then kill it
        bool fTriggerBlock = false;
        if (iType == WeaponDamageType::Feather && player->powerup == 3 && player->extrajumps > 0) {
            fTriggerBlock = true;
        } else if (iType == WeaponDamageType::PWings && player->powerup == 8 && player->flying) {
            fTriggerBlock = true;
        } else if (iType == WeaponDamageType::Star && player->isInvincible()) {
            fTriggerBlock = true;
        }

        if (fTriggerBlock)
            triggerBehavior(player->globalID, player->teamID);

        return IO_Block::hitbottom(player, useBehavior);
    }

    return false;
}

bool B_WeaponBreakableBlock::hitleft(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        if (iType == WeaponDamageType::Star && player->isInvincible())
            triggerBehavior(player->globalID, player->teamID);

        return IO_Block::hitleft(player, useBehavior);
    }

    return false;
}

bool B_WeaponBreakableBlock::hitright(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        if (iType == WeaponDamageType::Star && player->isInvincible())
            triggerBehavior(player->globalID, player->teamID);

        return IO_Block::hitright(player, useBehavior);
    }

    return false;
}

bool B_WeaponBreakableBlock::hittop(IO_MovingObject * object)
{
    IO_Block::hittop(object);

    if ((state == 1  || state == 2) && object->bounce == GRAVITATION) {
        BounceMovingObject(object);
        return false;
    }

    if (state != 0)
        return true;

    MovingObjectType type = object->getMovingObjectType();

    if (iType == WeaponDamageType::Fireball && type == movingobject_fireball) {
        triggerBehavior(object->iPlayerID, object->iTeamID);
        removeifprojectile(object, false, true);
        return false;
    }

    return true;
}

bool B_WeaponBreakableBlock::hitbottom(IO_MovingObject * object)
{
    if (state != 0)
        return true;

    IO_Block::hitbottom(object);

    MovingObjectType type = object->getMovingObjectType();

    if (iType == WeaponDamageType::Fireball && type == movingobject_fireball) {
        triggerBehavior(object->iPlayerID, object->iTeamID);
        removeifprojectile(object, false, true);
        return false;
    }

    return true;
}

bool B_WeaponBreakableBlock::hitright(IO_MovingObject * object)
{
    if (state != 0)
        return true;

    IO_Block::hitright(object);

    return objecthitside(object);
}

bool B_WeaponBreakableBlock::hitleft(IO_MovingObject * object)
{
    if (state != 0)
        return true;

    IO_Block::hitleft(object);

    return objecthitside(object);
}

void B_WeaponBreakableBlock::triggerBehavior(short iPlayerID, short iTeamID)
{
    if (state == 0) {
        eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokengrayblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16);
        eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokengrayblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16);
        eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokengrayblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16);
        eyecandy[2].emplace<EC_FallingObject>(&rm->spr_brokengrayblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16);

        state = 1;
        ifSoundOnPlay(rm->sfx_breakblock);

        iBumpPlayerID = iPlayerID;
        iBumpTeamID = iTeamID;
    }
}

bool B_WeaponBreakableBlock::objecthitside(IO_MovingObject * object)
{
    MovingObjectType type = object->getMovingObjectType();

    if (iType == WeaponDamageType::Shell && ((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock || (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity()))) {
        short iPlayerID = -1;
        short iTeamID = -1;
        if (type == movingobject_shell) {
            CO_Shell * shell = (CO_Shell*)object;
            iPlayerID = shell->iPlayerID;
            iTeamID = shell->iTeamID;
        } else if (type == movingobject_throwblock) {
            CO_ThrowBlock * throwblock = (CO_ThrowBlock*)object;
            iPlayerID = throwblock->iPlayerID;
            iTeamID = throwblock->iTeamID;
        } else if (type == movingobject_throwbox) {
            CO_ThrowBox * throwbox = (CO_ThrowBox*)object;
            iPlayerID = throwbox->iPlayerID;
            iTeamID = throwbox->iTeamID;
        }

        triggerBehavior(iPlayerID, iTeamID);
        return false;
    } else if (iType == WeaponDamageType::Fireball && type == movingobject_fireball) {
        MO_Fireball * fireball = (MO_Fireball*)object;
        triggerBehavior(fireball->iPlayerID, fireball->iTeamID);
        removeifprojectile(object, false, true);
        return false;
    } else if (type == movingobject_attackzone) {
        MO_AttackZone * zone = (MO_AttackZone*)object;

        if ((zone->iStyle == KillStyle::Leaf && iType == WeaponDamageType::Leaf) || (zone->iStyle == KillStyle::Feather && iType == WeaponDamageType::Feather)) {
            triggerBehavior(zone->iPlayerID, zone->iTeamID);
            zone->Die();
            return false;
        }
    }

    return true;
}
