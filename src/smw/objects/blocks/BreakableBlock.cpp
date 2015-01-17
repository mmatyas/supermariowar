#include "BreakableBlock.h"

#include "eyecandy.h"
#include "GameValues.h"
#include "gfx.h"
#include "map.h"
#include "object.h"
#include "objectgame.h" // TODO: refactor CO_ThrowBox and remove this
#include "player.h"
#include "PlayerKillTypes.h"
#include "ResourceManager.h"

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;
extern CEyecandyContainer eyecandy[3];

extern void CheckSecret(short id);

//------------------------------------------------------------------------------
// class breakable block
//------------------------------------------------------------------------------

B_BreakableBlock::B_BreakableBlock(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed) :
    IO_Block(nspr, x, y)
{
    iw = (short)nspr->getWidth() >> 2;
    iNumSprites = iNumSpr;
    animationSpeed = aniSpeed;
    animationTimer = 0;
    animationWidth = (short)nspr->getWidth();
    drawFrame = 0;
}

void B_BreakableBlock::draw()
{
    if (state == 0)
        spr->draw(ix, iy, drawFrame, 0, iw, ih);
}

void B_BreakableBlock::update()
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

    if (++animationTimer >= animationSpeed) {
        animationTimer = 0;

        drawFrame += iw;
        if (drawFrame >= animationWidth) {
            drawFrame = 0;
        }
    }
}


bool B_BreakableBlock::hittop(CPlayer * player, bool useBehavior)
{
    IO_Block::hittop(player, useBehavior);

    if (state == 1 || state == 2) {
        short iKillType = player_kill_nonkill;
        if (iBumpPlayerID >= 0 && !player->IsInvincibleOnBottom() && (player->teamID != iBumpTeamID || game_values.teamcollision == 2))
            iKillType = PlayerKilledPlayer(iBumpPlayerID, player, death_style_jump, kill_style_bounce, false, false);

        if (player_kill_nonkill == iKillType)
            player->vely = -VELNOTEBLOCKREPEL;
    } else if (useBehavior) {
        player->vely = GRAVITATION;

        if (player->IsSuperStomping() && state == 0) {
            triggerBehavior();
            return true;
        }
    }

    return false;
}

bool B_BreakableBlock::hitbottom(CPlayer * player, bool useBehavior)
{
    if (useBehavior && state == 0) {
        triggerBehavior();

        //When breaking a block, you smash through with a small velocity, but this allows for breaking two blocks at once
        /*
        if (player->vely < -VELMAXBREAKBLOCK)
        	player->vely = -VELMAXBREAKBLOCK;
        */
        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
        player->setYf((float)(iposy + ih) + 0.2f);

        iBumpPlayerID = player->globalID;
        iBumpTeamID = player->teamID;
    }

    return false;
}

bool B_BreakableBlock::hittop(IO_MovingObject * object)
{
    object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
    object->fOldY = object->fy;

    if (state == 0) {
        MovingObjectType type = object->getMovingObjectType();
        if (type == movingobject_throwbox && ((CO_ThrowBox*)object)->HasKillVelocity()) {
            triggerBehavior();
            object->vely = object->BottomBounce();
            return true;
        }
    }

    if ((state == 1  || state == 2) && object->bounce == GRAVITATION) {
        BounceMovingObject(object);
        return false;
    } else {
        object->vely = object->BottomBounce();
    }

    return true;
}

bool B_BreakableBlock::hitright(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx + iw) + 0.2f);
        object->fOldX = object->fx;

        if (object->velx < 0.0f)
            object->velx = -object->velx;

        MovingObjectType type = object->getMovingObjectType();
        if ((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock || (type == movingobject_throwbox && ((CO_ThrowBox*)object)->HasKillVelocity()) || type == movingobject_attackzone) {
            triggerBehavior();
            return true;
        }
    }

    return false;
}

bool B_BreakableBlock::hitleft(IO_MovingObject * object)
{
    if (state == 0) {
        object->setXf((float)(iposx - object->collisionWidth) - 0.2f);
        object->fOldX = object->fx;

        if (object->velx > 0.0f)
            object->velx = -object->velx;

        MovingObjectType type = object->getMovingObjectType();
        if ((type == movingobject_shell && object->state == 1) || type == movingobject_throwblock || (type == movingobject_throwbox && ((CO_ThrowBox*)object)->HasKillVelocity()) || type == movingobject_attackzone) {
            triggerBehavior();
            return true;
        }
    }

    return false;
}

void B_BreakableBlock::triggerBehavior()
{
    if (state == 0) {
        eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy, -2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
        eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy, 2.2f, -10.0f, 4, 2, 0, 0, 16, 16));
        eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix, iy + 16, -2.2f, -5.5f, 4, 2, 0, 0, 16, 16));
        eyecandy[2].add(new EC_FallingObject(&rm->spr_brokenyellowblock, ix + 16, iy + 16, 2.2f, -5.5f, 4, 2, 0, 0, 16, 16));

        state = 1;
        ifSoundOnPlay(rm->sfx_breakblock);
    }

    game_values.unlocksecret1part2++;
    CheckSecret(0);
}
