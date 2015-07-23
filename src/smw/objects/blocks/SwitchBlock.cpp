#include "SwitchBlock.h"

#include "map.h"
#include "objects/moving/MovingObject.h"
#include "player.h"

extern CMap* g_map;

B_SwitchBlock::B_SwitchBlock(gfxSprite *nspr, short x, short y, short colorID, short iState) :
    IO_Block(nspr, x, y)
{
    iw = (short)spr->getWidth() >> 2;
    collisionWidth = iw;
    ih = (short)spr->getHeight() >> 2;
    collisionHeight = ih;

    state = 1 - iState;
    iSrcX = colorID * 32;
}

void B_SwitchBlock::draw()
{
    spr->draw(ix, iy, iSrcX, (state == 0 ? 64 : 96), iw, ih);
}

bool B_SwitchBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
    if (state != 0 || !useBehavior)
        return true;

    if (player->fOldY + PH <= iposy && direction == 2)
        return hittop(player, useBehavior);
    else if (player->fOldY >= iposy + ih && direction == 0)
        return hitbottom(player, useBehavior);
    else if (player->fOldX + PW <= iposx && direction == 1)
        return hitleft(player, useBehavior);
    else if (player->fOldX >= iposx + iw && direction == 3)
        return hitright(player, useBehavior);

    return true;
}

bool B_SwitchBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);
    return false;
}

bool B_SwitchBlock::hitbottom(CPlayer * player, bool)
{
    player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
    player->setYf((float)(iposy + ih) + 0.2f);
    vely = -VELBLOCKBOUNCE;
    return false;
}

bool B_SwitchBlock::hitright(CPlayer * player, bool)
{
    player->setXf((float)(iposx + iw) + 0.2f);
    player->fOldX = player->fx;

    if (player->velx < 0.0f)
        player->velx = 0.0f;

    if (player->oldvelx < 0.0f)
        player->oldvelx = 0.0f;

    return false;
}

bool B_SwitchBlock::hitleft(CPlayer * player, bool)
{
    player->setXf((float)(iposx - PW) - 0.2f);
    player->fOldX = player->fx;

    if (player->velx > 0.0f)
        player->velx = 0.0f;

    if (player->oldvelx > 0.0f)
        player->oldvelx = 0.0f;

    return false;
}

bool B_SwitchBlock::hittop(IO_MovingObject * object)
{
    if (state == 0) {
        object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
        object->fOldY = object->fy;
        object->vely = object->BottomBounce();
    }

    return true;
}

bool B_SwitchBlock::hitbottom(IO_MovingObject * object)
{
    if (state == 0) {
        object->setYf((float)(iposy + ih) + 0.2f);
        object->vely = -object->vely;
    }

    return true;
}

bool B_SwitchBlock::hitright(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx + iw) + 0.2f);

        if (object->velx < 0.0f)
            object->velx = -object->velx;
    }

    return true;
}

bool B_SwitchBlock::hitleft(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx - object->collisionWidth) - 0.2f);

        if (object->velx > 0.0f)
            object->velx = -object->velx;
    }

    return true;
}

void B_SwitchBlock::FlipState(short playerID)
{
    state = 1 - state;
    g_map->UpdateTileGap(col, row);

    if (state == 0)
        KillPlayersAndObjectsInsideBlock(playerID);
}
