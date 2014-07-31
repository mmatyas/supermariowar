#include "PowerupBlock.h"

#include "GameValues.h"
#include "GameMode.h"
#include "gfx.h"
#include "map.h"
#include "object.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"

#include <cmath>

extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

#include "objectgame.h" // TODO: refactor CO_ThrowBox and remove this
extern IO_MovingObject* createpowerup(short iType, short ix, short iy, bool side, bool spawn);

//------------------------------------------------------------------------------
// class powerup block
//------------------------------------------------------------------------------

B_PowerupBlock::B_PowerupBlock(gfxSprite* nspr1, short x, short y, short iNumSpr, short aniSpeed, bool fHidden, short * piSettings)
    : IO_Block(nspr1, x, y)
{
    iw = (short)spr->getWidth() >> 2;
    ih = (short)spr->getHeight() >> 1;  //This sprite has two images (unused and used blocks)
    collisionWidth = iw;
    timer = 0;
    side = true;
    iNumSprites = iNumSpr;
    animationSpeed = aniSpeed;
    animationTimer = 0;
    animationWidth = (short)spr->getWidth();
    drawFrame = 0;

    hidden = ishiddentype = fHidden;

    if (piSettings[0] == -1 || game_values.overridepowerupsettings == 1) { //Game Only
        for (short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
            settings[iSetting] = game_values.powerupweights[iSetting];
    } else if (game_values.overridepowerupsettings == 0) { //Map Only
        for (short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
            settings[iSetting] = piSettings[iSetting];
    } else if (game_values.overridepowerupsettings == 2) { //Average
        for (short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++)
            settings[iSetting] = piSettings[iSetting] + game_values.powerupweights[iSetting];
    } else if (game_values.overridepowerupsettings == 3) { //Weighted
        float dMapWeightCount = 0;
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
            dMapWeightCount += piSettings[iPowerup];

        float dGameWeightCount = 0;
        for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
            dGameWeightCount += game_values.powerupweights[iPowerup];

        for (short iSetting = 0; iSetting < NUM_POWERUPS; iSetting++) {
            float dWeight = ((float)piSettings[iSetting] / dMapWeightCount + (float)game_values.powerupweights[iSetting] / dGameWeightCount) * 100.0f;

            //Cap lowest value at 1
            if (dWeight < 1.0f && dWeight > 0.0f)
                settings[iSetting] = 1;
            else
                settings[iSetting] = (short)dWeight;
        }
    }

    iCountWeight = 0;
    for (short iPowerup = 0; iPowerup < NUM_POWERUPS; iPowerup++)
        iCountWeight += settings[iPowerup];
}

B_PowerupBlock::~B_PowerupBlock()
{
}

void B_PowerupBlock::draw()
{
    if (!hidden)
        spr->draw(ix, iy, drawFrame, state == 0 ? 0 : ih, iw, ih);
}

void B_PowerupBlock::update()
{
    IO_Block::update();

    if (state > 0) {
        setYf(fy + vely);

        if (state == 1 && fabsf(fposy - fy) > 10.0f) {
            vely = -vely;
            state = 2;
            iBumpPlayerID = -1;
        } else if (state == 2 && fabsf(fposy - fy) < VELBLOCKBOUNCE) {
            vely = 0.0f;
            state = 3;
            setYi(iposy);

            if (game_values.gamemode->gamemode == game_mode_health && RANDOM_INT(100) < game_values.gamemodesettings.health.percentextralife) {
                createpowerup(HEALTH_POWERUP, ix, iy, side, true);
            } else if ((game_values.gamemode->gamemode == game_mode_timelimit && RANDOM_INT(100) < game_values.gamemodesettings.time.percentextratime) ||
                      (game_values.gamemode->gamemode == game_mode_star && RANDOM_INT(100) < game_values.gamemodesettings.star.percentextratime)) {
                createpowerup(TIME_POWERUP, ix, iy, side, true);
            } else if ((game_values.gamemode->gamemode == game_mode_coins && RANDOM_INT(100) < game_values.gamemodesettings.coins.percentextracoin) ||
                      (game_values.gamemode->gamemode == game_mode_greed && RANDOM_INT(100) < game_values.gamemodesettings.greed.percentextracoin)) {
                createpowerup(COIN_POWERUP, ix, iy, side, true);
            } else if (game_values.gamemode->gamemode == game_mode_jail && (RANDOM_INT(100)) < game_values.gamemodesettings.jail.percentkey) {
                createpowerup(JAIL_KEY_POWERUP, ix, iy, side, true);
            } else {
                createpowerup(SelectPowerup(), ix, iy, side, true);
            }

            ifSoundOnPlay(rm->sfx_sprout);
        } else if (state == 3) {
            if (game_values.itemrespawntime > 0 && ++timer >= game_values.itemrespawntime) {
                reset();
            }
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

void B_PowerupBlock::reset()
{
    timer = 0;
    state = 0;
}

bool B_PowerupBlock::collide(CPlayer* player, short direction, bool useBehavior)
{
    if (hidden) {
        if (player->fOldY >= iposy + ih && direction == 0)
            return hitbottom(player, useBehavior);

        return true;
    }

    return IO_Block::collide(player, direction, useBehavior);
}

bool B_PowerupBlock::hittop(CPlayer* player, bool useBehavior)
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

        if (state == 0) {
            if (player->IsSuperStomping()) {
                state = 1;
                vely = -VELBLOCKBOUNCE;
                side = player->ix + HALFPW < ix + (iw >> 1);
            }
        }
    }

    return false;
}

bool B_PowerupBlock::hitbottom(CPlayer* player, bool useBehavior)
{
    if (useBehavior) {
        //Player bounces off
        player->vely = CapFallingVelocity(-player->vely * BOUNCESTRENGTH);
        player->setYf((float)(iposy + ih) + 0.2f);

        if (hidden) {
            hidden = false;
            KillPlayersAndObjectsInsideBlock(player->globalID);
        }

        g_map->UpdateTileGap(col, row);

        if (state == 0) {
            ifSoundOnPlay(rm->sfx_bump);

            iBumpPlayerID = player->globalID;
            iBumpTeamID = player->teamID;

            vely = -VELBLOCKBOUNCE;
            state = 1;
            side = player->ix + HALFPW < ix + (iw >> 1);
        }
    }

    return false;
}

bool B_PowerupBlock::collide(IO_MovingObject* object, short direction)
{
    if (hidden)
        return true;

    return IO_Block::collide(object, direction);
}

bool B_PowerupBlock::hittop(IO_MovingObject* object)
{
    object->setYf((float)(iposy - object->collisionHeight) - 0.2f);
    object->fOldY = object->fy;

    MovingObjectType type = object->getMovingObjectType();
    if (type == movingobject_throwbox && ((CO_ThrowBox*)object)->HasKillVelocity()) {
        if (state == 0) {
            ifSoundOnPlay(rm->sfx_bump);

            iBumpPlayerID = -1;
            vely = -VELBLOCKBOUNCE;
            state = 1;
            side = false;
        }

        object->vely = object->BottomBounce();
    } else if (state == 1 && object->bounce == GRAVITATION) {
        BounceMovingObject(object);
        return false;
    } else {
        object->vely = object->BottomBounce();
    }

    return true;
}

bool B_PowerupBlock::hitright(IO_MovingObject* object)
{
    //Object bounces off
    object->setXf((float)(iposx + iw) + 0.2f);
    object->fOldX = object->fx;

    if (object->velx < 0.0f)
        object->velx = -object->velx;

    MovingObjectType type = object->getMovingObjectType();
    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone) {
        if (type == movingobject_shell) {
            if (object->state != 1)
                return false;
        }

        //If the box isn't moving fast enough, then don't do anything
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return false;

        if (state == 0) {
            ifSoundOnPlay(rm->sfx_bump);

            iBumpPlayerID = -1;
            vely = -VELBLOCKBOUNCE;
            state = 1;
            side = false;
        }

        return true;
    }

    return false;
}

bool B_PowerupBlock::hitleft(IO_MovingObject* object)
{
    //Object bounces off
    object->setXf((float)(iposx - object->collisionWidth) - 0.2f);
    object->fOldX = object->fx;

    if (object->velx > 0.0f)
        object->velx = -object->velx;

    MovingObjectType type = object->getMovingObjectType();
    if (type == movingobject_shell || type == movingobject_throwblock || type == movingobject_throwbox || type == movingobject_attackzone) {
        if (type == movingobject_shell) {
            if (object->state != 1)
                return false;
        }

        //If the box isn't moving fast enough, then don't do anything
        if (type == movingobject_throwbox && !((CO_ThrowBox*)object)->HasKillVelocity())
            return false;

        if (state == 0) {
            ifSoundOnPlay(rm->sfx_bump);

            iBumpPlayerID = -1;
            vely = -VELBLOCKBOUNCE;
            state = 1;
            side = true;
        }

        return true;
    }

    return false;
}

void B_PowerupBlock::triggerBehavior()
{
    if (state == 0) {
        ifSoundOnPlay(rm->sfx_bump);

        iBumpPlayerID = -1;
        vely = -VELBLOCKBOUNCE;
        state = 1;
        side = true;
    }
}

short B_PowerupBlock::SelectPowerup()
{
    if (iCountWeight == 0)
        return -1;

    int iRandPowerup = RANDOM_INT(iCountWeight + 1);
    int iSelectedPowerup = 0;

    int iPowerupWeightCount = settings[iSelectedPowerup];

    while (iPowerupWeightCount < iRandPowerup)
        iPowerupWeightCount += settings[++iSelectedPowerup];

    return iSelectedPowerup;
}
