#include "MO_FlagBase.h"

#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "ObjectContainer.h"
#include "player.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "objects/carriable/CO_Flag.h"

#include <cmath>

extern CObjectContainer objectcontainer[3];
extern CMap* g_map;
extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class flag base (for CTF mode)
//------------------------------------------------------------------------------
MO_FlagBase::MO_FlagBase(gfxSprite* nspr, short iTeamID, short iColorID)
    : IO_MovingObject(nspr, 1280, 960, 5, 0)  // use 1280 and 960 so when placing base, it doesn't interfere (look in getClosestObject())
{
    state = 1;
    iw = 32;
    ih = 32;
    collisionWidth = 32;
    collisionHeight = 32;

    objectType = object_moving;
    movingObjectType = movingobject_flagbase;
    teamID = iTeamID;
    iGraphicOffsetX = iColorID * 48;

    angle = (float)(RANDOM_INT(1000) * 0.00628f);
    anglechange = (float)(RANDOM_INT(100) * 0.0002f);
    anglechangetimer = (short)(RANDOM_INT(50) + 100);

    velx = sin(angle);
    vely = cos(angle);

    homeflag = NULL;

    placeFlagBase(true);

    speed = (float)game_values.gamemodesettings.flag.speed / 4.0f;

    timer = 0;

    fObjectCollidesWithMap = false;
}

bool MO_FlagBase::collide(CPlayer* player)
{
    if (teamID == player->teamID && player->carriedItem && player->carriedItem->getMovingObjectType() == movingobject_flag) {
        CO_Flag* flag = (CO_Flag*)player->carriedItem;
        scoreFlag(flag, player);
        timer = 0;
    }

    return false;
}

void MO_FlagBase::draw()
{
    spr->draw(ix - 8, iy - 8, iGraphicOffsetX, 0, 48, 48);
}


void MO_FlagBase::update()
{
    if (game_values.gamemodesettings.flag.speed > 0) {
        if (--anglechangetimer <= 0) {
            anglechange = (float)((RANDOM_INT(101) - 50) * 0.0002f);
            anglechangetimer = (short)(RANDOM_INT(50) + 100);
        }

        angle += anglechange;

        velx = speed * sin(angle);
        vely = speed * cos(angle);

        setXf(fx + velx);
        setYf(fy + vely);

        if (ix < 0) {
            velx = -velx;
            ix = 0;
            fx = (float)ix;

            angle = atan2(velx, vely);
        } else if (ix + collisionWidth >= App::screenWidth) {
            velx = -velx;
            ix = App::screenWidth - 1 - collisionWidth;
            fx = (float)ix;

            angle = atan2(velx, vely);
        }

        if (iy < 0) {
            vely = -vely;
            iy = 0;
            fy = (float)iy;

            angle = atan2(velx, vely);
        } else if (iy + collisionHeight >= App::screenHeight) {
            vely = -vely;
            iy = App::screenHeight - 1 - collisionHeight;
            fy = (float)iy;

            angle = atan2(velx, vely);
        }
    }

    if (game_values.gamemodesettings.flag.speed == 0 && timer++ > 1000)
        placeFlagBase(false);
}

void MO_FlagBase::placeFlagBase(bool fInit)
{
    timer = 0;
    short x = 0, y = 0;

    if (fInit && teamID < g_map->iNumFlagBases) {
        x = g_map->flagbaselocations[teamID].x;
        y = g_map->flagbaselocations[teamID].y;
    } else {
        short iAttempts = 32;
        while ((!g_map->findspawnpoint(5, &x, &y, collisionWidth, collisionHeight, false) || objectcontainer[1].getClosestMovingObject(x, y, movingobject_flagbase) < 200.0f)
            && iAttempts-- > 0)
            ;
    }

    setXi(x);
    setYi(y);
}

void MO_FlagBase::collide(IO_MovingObject* object)
{
    if (object->getMovingObjectType() == movingobject_flag) {
        CO_Flag* flag = (CO_Flag*)object;
        CPlayer* player = flag->owner_throw;

        if (player) {
            if (teamID == player->teamID) {
                scoreFlag(flag, player);
            }
        }
    }
}

void MO_FlagBase::scoreFlag(CO_Flag* flag, CPlayer* player)
{
    if (flag->teamID == teamID) {
        flag->placeFlag();
        ifSoundOnPlay(rm->sfx_areatag);
    } else if (!game_values.gamemodesettings.flag.homescore || homeflag != NULL || game_values.gamemodesettings.flag.centerflag) {
        flag->placeFlag();
        if (!game_values.gamemode->gameover) {
            player->Score().AdjustScore(1);
            game_values.gamemode->CheckWinner(player);
        }

        ifSoundOnPlay(rm->sfx_racesound);

        if (game_values.gamemodesettings.flag.pointmove) {
            // Set the values way outside the map so it will place the base correctly
            ix = 1280;
            iy = 960;
            placeFlagBase(false);
        }
    }
}
