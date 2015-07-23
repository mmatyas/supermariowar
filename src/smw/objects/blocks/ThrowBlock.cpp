#include "ThrowBlock.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"

#include "objects/carriable/ThrowBlock.h"

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;
extern CEyecandyContainer eyecandy[3];
extern CObjectContainer objectcontainer[3];

B_ThrowBlock::B_ThrowBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short type) :
    IO_Block(nspr, x, y)
{
    iw = 32;
    ih = 32;
    iNumSprites = iNumSpr;
    animationSpeed = aniSpeed;
    animationTimer = 0;
    drawFrame = 0;
    animationWidth = (short)nspr->getWidth();
    iType = type;
}

void B_ThrowBlock::draw()
{
    spr->draw(ix, iy, drawFrame, iType << 5, iw, ih);
}

void B_ThrowBlock::update()
{
    if (++animationTimer >= animationSpeed) {
        animationTimer = 0;

        drawFrame += iw;
        if (drawFrame >= animationWidth) {
            drawFrame = 0;
        }
    }
}

bool B_ThrowBlock::hittop(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setYf((float)(iposy - PH) - 0.2f);
        player->fOldY = player->fy;
        player->inair = false;
        player->fallthrough = false;
        player->killsinrowinair = 0;
        player->extrajumps = 0;
        player->vely = GRAVITATION;

        if (player->PressedAcceptItemKey() && player->IsAcceptingItem()) {
            GiveBlockToPlayer(player);
            return true;
        } else {
            return false;
        }
    }

    return false;
}

bool B_ThrowBlock::hitright(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx + iw) + 0.2f);
        player->fOldX = player->fx;

        if (player->velx < 0.0f)
            player->velx = 0.0f;

        if (player->oldvelx < 0.0f)
            player->oldvelx = 0.0f;

        if (player->IsAcceptingItem()) {
            GiveBlockToPlayer(player);
            return true;
        } else {
            return false;
        }
    }

    return true;
}

bool B_ThrowBlock::hitleft(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx - PW) - 0.2f);
        player->fOldX = player->fx;

        if (player->velx > 0.0f)
            player->velx = 0.0f;

        if (player->oldvelx > 0.0f)
            player->oldvelx = 0.0f;

        if (player->IsAcceptingItem()) {
            GiveBlockToPlayer(player);
            return true;
        } else {
            return false;
        }
    }

    return true;
}

void B_ThrowBlock::GiveBlockToPlayer(CPlayer * player)
{
    CO_ThrowBlock * block = new CO_ThrowBlock(&rm->spr_blueblock, ix, iy, iType);
    if (player->AcceptItem(block)) {
        dead = true;
        g_map->blockdata[col][row] = NULL;
        g_map->UpdateTileGap(col, row);

        block->owner = player;
        block->iPlayerID = player->globalID;
        objectcontainer[1].add(block);
    } else {
        delete block;
    }
}

void B_ThrowBlock::triggerBehavior()
{
    dead = true;
    g_map->blockdata[col][row] = NULL;
    g_map->UpdateTileGap(col, row);

    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy, -1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy, 1.5f, -7.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix, iy + 16, -1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));
    eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenblueblock, ix + 16, iy + 16, 1.5f, -4.0f, 6, 2, 0, iType << 4, 16, 16));

    ifSoundOnPlay(rm->sfx_breakblock);
}
