#include "BounceBlock.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "map.h"
#include "player.h"
#include "PlayerKillTypes.h"
#include "ResourceManager.h"

#include "objects/moving/MovingObject.h"

#include <cmath>

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

B_BounceBlock::B_BounceBlock(gfxSprite *nspr1, short x, short y, bool fHidden) :
    IO_Block(nspr1, x, y)
{
    hidden = ishiddentype = fHidden;
}

void B_BounceBlock::draw()
{
    if (hidden)
        return;

    IO_Block::draw();
}

void B_BounceBlock::update()
{
    IO_Block::update();

    if (state > 0) {
        setYf(fy + vely);

        if (state == 1 && fabsf(fposy - fy) > 10.0f) {
            iBumpPlayerID = -1;
            vely = -vely;
            state = 2;
        } else if (state == 2 && fabsf(fposy - fy) < VELBLOCKBOUNCE) {
            reset();
        }
    }
}

void B_BounceBlock::reset()
{
    vely = 0.0f;
    state = 0;
    setYf(fposy);
}

bool B_BounceBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
    if (hidden) {
        if (player->fOldY >= iposy + ih && direction == 0)
            return hitbottom(player, useBehavior);

        return true;
    }

    return IO_Block::collide(player, direction, useBehavior);
}

bool B_BounceBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);

    if (state == 1) {
        short iKillType = player_kill_nonkill;
        if (iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
            iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false, false);

        if (player_kill_nonkill == iKillType)
            player->vely = -VELNOTEBLOCKREPEL;
    } else if (useBehavior) {
        player->vely = GRAVITATION;
    }

    return false;
}

bool B_BounceBlock::hitbottom(CPlayer * player, bool useBehavior)
{
    //Player bounces off
    if (useBehavior) {
        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
        player->setYf((float)(iposy + ih) + 0.2f);

        iBumpPlayerID = player->globalID;
        iBumpTeamID = player->teamID;

        triggerBehavior();

        if (hidden) {
            hidden = false;
            KillPlayersAndObjectsInsideBlock(iBumpPlayerID);
        }

        g_map->UpdateTileGap(col, row);
    }

    return false;
}

bool B_BounceBlock::collide(IO_MovingObject * object, short direction)
{
    if (hidden)
        return true;

    return IO_Block::collide(object, direction);
}

bool B_BounceBlock::hittop(IO_MovingObject * object)
{
    object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
    object->fOldY = object->fy;

    if (state == 1 && object->bounce == GRAVITATION) {
        BounceMovingObject(object);
        return false;
    } else {
        object->vely = object->BottomBounce();
    }

    return true;
}

void B_BounceBlock::triggerBehavior()
{
    if (state == 0) {
        vely = -VELBLOCKBOUNCE;
        state = 1;
        ifSoundOnPlay(rm->sfx_bump);
    }
}
