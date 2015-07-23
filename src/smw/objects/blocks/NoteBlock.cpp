#include "NoteBlock.h"

#include "GameValues.h"
#include "GlobalConstants.h"
#include "gfx.h"
#include "map.h"
#include "object.h"
#include "player.h"
#include "ResourceManager.h"

#include <math.h>

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

extern void CheckSecret(short id);

B_NoteBlock::B_NoteBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type, bool fHidden) :
    IO_Block(nspr, x, y)
{
    iw = (short)nspr->getWidth() >> 2;
    ih = TILESIZE;
    iNumSprites = iNumSpr;
    animationSpeed = aniSpeed;
    animationTimer = 0;
    drawFrame = 0;
    animationWidth = (short)spr->getWidth();

    hidden = ishiddentype = fHidden;

    iType = type;
    iTypeOffsetY = iType << 5;
}

void B_NoteBlock::draw()
{
    if (hidden)
        return;

    spr->draw(ix, iy, drawFrame, iTypeOffsetY, iw, ih);
}

void B_NoteBlock::update()
{
    IO_Block::update();

    if (state > 0) {
        setXf(fx + velx);
        setYf(fy + vely);

        if (state == 1 && fabsf(fposx - fx) > 10.0f) {
            velx = -velx;
            state = 2;
        } else if (state == 2 && fabsf(fposx - fx) < VELNOTEBLOCKBOUNCE) {
            reset();
        } else if (state == 3 && fabsf(fposy - fy) > 10.0f) {
            vely = -vely;
            state = 4;
            iBumpPlayerID = -1;
        } else if (state == 4 && fabsf(fposy - fy) < VELNOTEBLOCKBOUNCE) {
            reset();
        }
    }

    if (++animationTimer >= animationSpeed) {
        animationTimer = 0;

        drawFrame += iw;
        if (drawFrame >= animationWidth) {
            drawFrame = 0;
        }
    }
}

void B_NoteBlock::reset()
{
    velx = 0.0f;
    vely = 0.0f;
    state = 0;
    setXf(fposx);
    setYf(fposy);
}

bool B_NoteBlock::collide(CPlayer * player, short direction, bool useBehavior)
{
    if (hidden) {
        if ((player->fOldY >= iposy + ih || state > 1) && direction == 0)
            return hitbottom(player, useBehavior);

        return true;
    }

    return IO_Block::collide(player, direction, useBehavior);
}

bool B_NoteBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);

    if (useBehavior) {
        player->superjumptimer = 4;
        player->superjumptype = iType;
        player->vely = -VELNOTEBLOCKREPEL;

        if (state == 0) {
            vely = VELNOTEBLOCKBOUNCE;
            state = 3;
        }

        ifSoundOnPlay(rm->sfx_bump);

        game_values.unlocksecret3part2[player->globalID] += 2;
        CheckSecret(2);
    }

    return false;
}

bool B_NoteBlock::hitbottom(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setYf((float)(iposy + ih) + 0.2f);
        player->vely = VELNOTEBLOCKREPEL;

        if (state == 0) {
            iBumpPlayerID = player->globalID;
            iBumpTeamID = player->teamID;

            vely = -VELNOTEBLOCKBOUNCE;
            state = 3;

            ifSoundOnPlay(rm->sfx_bump);
        }

        if (hidden) {
            hidden = false;
            KillPlayersAndObjectsInsideBlock(player->globalID);
        }

        g_map->UpdateTileGap(col, row);
    }

    return false;
}

bool B_NoteBlock::hitright(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx + iw) + 0.2f);
        player->fOldX = player->fx;
        player->velx = VELNOTEBLOCKREPEL;
        player->oldvelx = VELNOTEBLOCKREPEL;

        if (state == 0) {
            velx = -VELNOTEBLOCKBOUNCE;
            state = 1;
        }

        ifSoundOnPlay(rm->sfx_bump);
    }

    return false;
}

bool B_NoteBlock::hitleft(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx - PW) - 0.2f);
        player->fOldX = player->fx;
        player->velx = -VELNOTEBLOCKREPEL;
        player->oldvelx = -VELNOTEBLOCKREPEL;

        if (state == 0) {
            velx = VELNOTEBLOCKBOUNCE;
            state = 1;
        }

        ifSoundOnPlay(rm->sfx_bump);
    }

    return false;
}

bool B_NoteBlock::collide(IO_MovingObject * object, short direction)
{
    if (hidden)
        return true;

    return IO_Block::collide(object, direction);
}

bool B_NoteBlock::hittop(IO_MovingObject * object)
{
    object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
    object->fOldY = object->fy;

    if (state == 3 && object->bounce == GRAVITATION) {
        BounceMovingObject(object);
        return false;
    } else {
        object->vely = object->BottomBounce();
    }

    return true;
}
