#include "FlipBlock.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "map.h"
#include "objects/moving/MovingObject.h"
#include "objects/carriable/ThrowBox.h"
#include "player.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern CResourceManager* rm;
extern CGameValues game_values;
extern CEyecandyContainer eyecandy[3];

//------------------------------------------------------------------------------
// class flip block
// Known Bug:  If you have two flip blocks vertically aligned like this:
//
// ['']  or  ['']['']  etc.
// ['']      ['']['']
//
// And you hit both of them from the bottom, then keep jumping so that the bottom one
// stops to support you, and then the top one stops spinning, you can "hide" under the
// top block.  I was thinking about fixing this, but I don't know what the correct
// behavior should be if you're in a flip block when it stops spinning.  Plus, it is kind
// of cool to hide under these blocks.  You are somewhat invincible, but someone can come
// along and hit your support out from under you and you'll fall out and if they have star
// power, you're dead.  Plus fireballs have a large enough collision box that you can be hit
// with them too.
//------------------------------------------------------------------------------
B_FlipBlock::B_FlipBlock(gfxSprite *nspr, short x, short y, bool fHidden) :
    IO_Block(nspr, x, y)
{
    iw = (short)spr->getWidth() >> 2;
    collisionWidth = iw;

    hidden = ishiddentype = fHidden;

    counter = 0;
    frame = 0;
    timer = 0;
    animationWidth = (short)spr->getWidth();
}

void B_FlipBlock::draw()
{
    if (hidden)
        return;

    if (state == 0 || state == 1)
        spr->draw(ix, iy, frame, 0, iw, ih);
}

void B_FlipBlock::update()
{
    IO_Block::update();

    if (state == 1) {
        if (++counter >= 10) {
            counter = 0;
            frame += iw;

            if (frame >= animationWidth) {
                frame = 0;
            }
        }

        if (++timer >= 240) {
            reset();
            g_map->UpdateTileGap(col, row);

            KillPlayersAndObjectsInsideBlock(iBumpPlayerID);
            iBumpPlayerID = -1;
        }
    } else if (state == 2) {
        state = 3;
    } else if (state == 3) {
        dead = true;
        g_map->blockdata[col][row] = NULL;
        g_map->UpdateTileGap(col, row);
    }
}

void B_FlipBlock::reset()
{
    frame = 0;
    counter = 0;
    timer = 0;
    state = 0;
}

bool B_FlipBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
    if (hidden) {
        if (player->fOldY >= iposy + ih && direction == 0)
            return hitbottom(player, useBehavior);

        return true;
    }

    if ((player->fOldY + PH <= iposy || state > 1) && direction == 2)
        return hittop(player, useBehavior);
    else if ((player->fOldY >= iposy + ih || state > 1) && direction == 0)
        return hitbottom(player, useBehavior);
    else if ((player->fOldX + PW <= iposx || state > 1) && direction == 1)
        return hitleft(player, useBehavior);
    else if ((player->fOldX >= iposx + iw || state > 1) && direction == 3)
        return hitright(player, useBehavior);

    return true;
}

bool B_FlipBlock::hittop(CPlayer * player, bool useBehavior)
{
    if (state == 2 || state == 3) {
        IO_Block::hittop(player, useBehavior);

        player->vely = -VELNOTEBLOCKREPEL;
        return false;
    } else if (state == 0) {
        IO_Block::hittop(player, useBehavior);

        if (player->IsSuperStomping()) {
            state = 2;
            explode();
            return true;
        }

        return false;
    }

    return true;
}

bool B_FlipBlock::hitbottom(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
        player->setYf((float)(iposy + ih) + 0.2f);

        iBumpPlayerID = player->globalID;
        iBumpTeamID = player->teamID;

        if (hidden) {
            hidden = false;
            KillPlayersAndObjectsInsideBlock(iBumpPlayerID);
        }

        g_map->UpdateTileGap(col, row);

        triggerBehavior();
        return false;
    }

    return true;
}

bool B_FlipBlock::hitright(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        IO_Block::hitright(player, useBehavior);
    }

    return true;
}

bool B_FlipBlock::hitleft(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        IO_Block::hitleft(player, useBehavior);
    }

    return true;
}

bool B_FlipBlock::collide(IO_MovingObject * object, short direction)
{
    if (hidden)
        return true;

    return IO_Block::collide(object, direction);
}

bool B_FlipBlock::hittop(IO_MovingObject * object)
{
    if (state == 0) {
        object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
        object->fOldY = object->fy;
        object->vely = object->BottomBounce();
    }

    return true;
}

bool B_FlipBlock::hitbottom(IO_MovingObject * object)
{
    if (state == 0) {
        object->setYf((float)(iposy + ih) + 0.2f);
        object->vely = -object->vely;
    }

    return true;
}

bool B_FlipBlock::hitright(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx + iw) + 0.2f);
        object->fOldX = object->fx;

        if (object->velx < 0.0f)
            object->velx = -object->velx;

        MovingObjectType type = object->getMovingObjectType();
        if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
            if (type == movingobject_shell) {
                if (object->state != 1)
                    return false;
            }

            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity()) {
                return false;
            }

            state = 2;
            explode();
        } else if (type == movingobject_attackzone) {
            iBumpPlayerID = object->iPlayerID;
            iBumpTeamID = object->iTeamID;

            if (hidden) {
                hidden = false;
                KillPlayersAndObjectsInsideBlock(iBumpPlayerID);
            }

            g_map->UpdateTileGap(col, row);

            triggerBehavior();
            return false;
        }
    }

    return true;
}

bool B_FlipBlock::hitleft(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx - object->collisionWidth) - 0.2f);
        object->fOldX = object->fx;

        if (object->velx > 0.0f)
            object->velx = -object->velx;

        MovingObjectType type = object->getMovingObjectType();
        if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox) {
            if (type == movingobject_shell) {
                if (object->state != 1)
                    return false;
            }

            if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity()) {
                return false;
            }

            state = 2;
            explode();
        } else if (type == movingobject_attackzone) {
            iBumpPlayerID = object->iPlayerID;
            iBumpTeamID = object->iTeamID;

            if (hidden) {
                hidden = false;
                KillPlayersAndObjectsInsideBlock(iBumpPlayerID);
            }

            g_map->UpdateTileGap(col, row);

            triggerBehavior();
            return false;
        }
    }

    return true;
}

void B_FlipBlock::triggerBehavior()
{
    if (state == 0) {
        state = 1;
        frame = iw;

        g_map->UpdateTileGap(col, row);
    }
}

void B_FlipBlock::explode()
{
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenflipblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenflipblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenflipblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenflipblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));

    ifSoundOnPlay(rm->sfx_breakblock);
}
