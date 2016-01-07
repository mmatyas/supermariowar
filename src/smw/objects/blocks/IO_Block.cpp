#include "IO_Block.h"

#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "player.h"
#include "ResourceManager.h"
#include "objects/moving/MovingObject.h"
#include "objects/moving/WalkingEnemy.h"
#include "objects/carriable/Shell.h"
#include "objects/carriable/ThrowBlock.h"
#include "objects/carriable/ThrowBox.h"

extern CMap* g_map;
extern CObjectContainer objectcontainer[3];

extern CGameValues game_values;
extern CResourceManager* rm;

extern CPlayer* list_players[4];
extern short list_players_cnt;

extern CPlayer* GetPlayerFromGlobalID(short iGlobalID);

//------------------------------------------------------------------------------
// class Block base class
//------------------------------------------------------------------------------

IO_Block::IO_Block(gfxSprite *nspr, short x, short y)
    : CObject(nspr, x, y)
{
    objectType = object_block;

    iBumpPlayerID = -1;
    iBumpTeamID = -1;

    fposx = fx;
    fposy = fy;

    iposx = x;
    iposy = y;

    col = x / TILESIZE;
    row = y / TILESIZE;

    hidden = ishiddentype = false;
    iHiddenTimer = 0;
}

void IO_Block::draw()
{
    spr->draw(ix, iy);
}

void IO_Block::update()
{
    if (ishiddentype && !hidden) {
        if (game_values.hiddenblockrespawn > 0 && ++iHiddenTimer > game_values.hiddenblockrespawn) {
            iHiddenTimer = 0;
            hidden = true;
            reset();

            g_map->UpdateTileGap(col, row);
        }
    }
}

void IO_Block::reset()
{
    state = 0;
}

bool IO_Block::collide(CPlayer * player, short direction, bool useBehavior)
{
    if (direction == 2)
        return hittop(player, useBehavior);
    else if (direction == 0)
        return hitbottom(player, useBehavior);
    else if (direction == 1)
        return hitleft(player, useBehavior);
    else
        return hitright(player, useBehavior);
}

bool IO_Block::hittop(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setYf((float)(iposy - PH) - 0.2f);
        player->inair = false;
        player->fallthrough = false;
        player->killsinrowinair = 0;
        player->extrajumps = 0;
        player->vely = GRAVITATION;
    }

    return false;
}

bool IO_Block::hitbottom(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        //Player bounces off
        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
        player->setYf((float)(iposy + ih) + 0.2f);
    }

    return false;
}

bool IO_Block::hitright(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx + iw) + 0.2f);
        player->fOldX = player->fx;

        if (player->velx < 0.0f)
            player->velx = 0.0f;

        if (player->oldvelx < 0.0f)
            player->oldvelx = 0.0f;
    }

    return false;
}

bool IO_Block::hitleft(CPlayer * player, bool useBehavior)
{
    if (useBehavior) {
        player->setXf((float)(iposx - PW) - 0.2f);
        player->fOldX = player->fx;

        if (player->velx > 0.0f)
            player->velx = 0.0f;

        if (player->oldvelx > 0.0f)
            player->oldvelx = 0.0f;
    }

    return false;
}

bool IO_Block::collide(IO_MovingObject * object, short direction)
{
    if (direction == 2)
        return hittop(object);
    else if (direction == 0)
        return hitbottom(object);
    else if (direction == 1)
        return hitleft(object);
    else
        return hitright(object);
}

bool IO_Block::hittop(IO_MovingObject * object)
{
    object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
    object->fOldY = object->fy;
    object->vely = object->BottomBounce();
    return true;
}

bool IO_Block::hitbottom(IO_MovingObject * object)
{
    object->setYf((float)(iposy + ih) + 0.2f);
    object->fOldY = object->fy;
    object->vely = -object->vely;
    return true;
}

bool IO_Block::hitright(IO_MovingObject * object)
{
    object->setXf((float)(iposx + iw) + 0.2f);
    object->fOldX = object->fx;

    if (object->velx < 0.0f)
        object->velx = -object->velx;

    return true;
}

bool IO_Block::hitleft(IO_MovingObject * object)
{
    object->setXf((float)(iposx - object->collisionWidth) - 0.2f);
    object->fOldX = object->fx;

    if (object->velx > 0.0f)
        object->velx = -object->velx;

    return true;
}

void IO_Block::BounceMovingObject(IO_MovingObject * object)
{
    MovingObjectType type = object->getMovingObjectType();
    if (type == movingobject_goomba || type == movingobject_koopa || type == movingobject_buzzybeetle || type == movingobject_spiny) {
        ifSoundOnPlay(rm->sfx_kicksound);

        MO_WalkingEnemy * enemy = (MO_WalkingEnemy *)object;
        killstyle style = enemy->getKillStyle();

        enemy->DieAndDropShell(true, true);

        if (!game_values.gamemode->gameover && iBumpPlayerID >= 0) {
            CPlayer * player = GetPlayerFromGlobalID(iBumpPlayerID);

            if (player) {
                player->AddKillerAward(NULL, style);
                player->score->AdjustScore(1);
            }
        }
    } else if (type == movingobject_shell) {
        CO_Shell * shell = (CO_Shell*)object;
        shell->Flip();
    } else if (type == movingobject_throwblock) {
        CO_ThrowBlock * block = (CO_ThrowBlock*)object;
        block->Die();
    } else if (type == movingobject_throwbox) {
        CO_ThrowBox * box = (CO_ThrowBox*)object;
        box->Die();
    } else {
        object->vely = -VELNOTEBLOCKREPEL;
    }
}

void IO_Block::KillPlayersAndObjectsInsideBlock(short playerID)
{
    //Loop through players
    for (short iPlayer = 0; iPlayer < list_players_cnt; iPlayer++) {
        CPlayer * player = list_players[iPlayer];

        if (!player->isready())
            continue;

        short iSwapSides = 0;
        if (player->fOldX >= iposx + TILESIZE)
            iSwapSides = -smw->ScreenWidth;

        if (player->fOldX + PW + iSwapSides >= iposx && player->fOldX + iSwapSides < iposx + TILESIZE &&
                player->fOldY + PH >= iposy && player->fOldY < iposy + TILESIZE) {
            player->iSuicideCreditPlayerID = playerID;
            player->iSuicideCreditTimer = 1;
            player->KillPlayerMapHazard(true, kill_style_environment, true);
        }
    }

    //Loop through objects
    for (short iLayer = 0; iLayer < 3; iLayer++) {
        short iContainerEnd = objectcontainer[iLayer].list_end;
        for (short iObject = 0; iObject < iContainerEnd; iObject++) {
            CObject * object = objectcontainer[iLayer].list[iObject];

            if (object->getObjectType() != object_moving)
                continue;

            IO_MovingObject * movingobject = (IO_MovingObject*)object;

            if (!movingobject->CollidesWithMap())
                continue;

            short iSwapSides = 0;
            if (movingobject->fOldX >= iposx + TILESIZE)
                iSwapSides = -smw->ScreenWidth;

            if (movingobject->fOldX + PW + iSwapSides >= iposx && movingobject->fOldX + iSwapSides < iposx + TILESIZE &&
                    movingobject->fOldY + PH >= iposy && movingobject->fOldY < iposy + TILESIZE) {
                movingobject->KillObjectMapHazard(playerID);
            }
        }
    }
}
