#include "objectgame.h"

#include "eyecandy.h"
#include "GameMode.h"
#include "GameValues.h"
#include "map.h"
#include "movingplatform.h"
#include "ObjectContainer.h"
#include "objecthazard.h"
#include "RandomNumberGenerator.h"
#include "ResourceManager.h"
#include "gamemodes/BonusHouse.h"
#include "gamemodes/Chase.h"
#include "gamemodes/Race.h"
#include "gamemodes/Star.h"
#include "gamemodes/MiniBoss.h"
#include "gamemodes/MiniBoxes.h"
#include "gamemodes/MiniPipe.h"

#include <cstdlib> // abs()
#include <cmath>

extern short iKingOfTheHillZoneLimits[4][4];
extern void PlayerKilledPlayer(short iKiller, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern void PlayerKilledPlayer(CPlayer * killer, CPlayer * killed, short deathstyle, short KillStyle, bool fForce, bool fKillCarriedItem);
extern bool SwapPlayers(short iUsingPlayerID);
extern short scorepowerupoffsets[3][3];

extern CPlayer * GetPlayerFromGlobalID(short iGlobalID);
extern void CheckSecret(short id);
extern SpotlightManager spotlightManager;
extern CObjectContainer objectcontainer[3];

extern CGM_Pipe_MiniGame * pipegamemode;

extern CPlayer* list_players[4];
extern short score_cnt;

extern CMap* g_map;
extern CEyecandyContainer eyecandy[3];

extern CGameValues game_values;
extern CResourceManager* rm;

//------------------------------------------------------------------------------
// class feather powerup
//------------------------------------------------------------------------------
PU_FeatherPowerup::PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY) :
    IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    desty = fy - collisionHeight;
    movingObjectType = movingobject_powerup;

    iw = (short)nspr->getWidth() >> 1;

    velx = 0.0f;
    fFloatDirectionRight = true;
    dFloatAngle = HALF_PI;

    fObjectCollidesWithMap = false;
}

void PU_FeatherPowerup::draw()
{
    if (state == 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, (short)(ih - fy + desty));
    else if (state == 1)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, 0, 0, iw, ih);
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, (fFloatDirectionRight ? 0 : 32), 0, iw, ih);
}


void PU_FeatherPowerup::update()
{
    //Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 4.0f);

        if (fy <= desty) {
            state = 1;
        }
    } else if (state == 1) {
        fOldX = fx;
        fOldY = fy;

        setYf(fy - 4.0f);

        if (fy <= desty - 128.0f) {
            state = 2;
            dFloatCenterY = fy - 64.0f;
            dFloatCenterX = fx;
        }
    } else {
        if (!fFloatDirectionRight) {
            dFloatAngle += 0.035f;

            if (dFloatAngle >= THREE_QUARTER_PI) {
                dFloatAngle = THREE_QUARTER_PI;
                fFloatDirectionRight = true;
            }
        } else {
            dFloatAngle -= 0.035f;

            if (dFloatAngle <= QUARTER_PI) {
                dFloatAngle = QUARTER_PI;
                fFloatDirectionRight = false;
            }
        }

        dFloatCenterY += 1.0f;

        setXf(64.0f * cos(dFloatAngle) + dFloatCenterX);
        setYf(64.0f * sin(dFloatAngle) + dFloatCenterY);

        if (fy >= App::screenHeight)
            dead = true;
    }
}

bool PU_FeatherPowerup::collide(CPlayer * player)
{
    if (state > 0) {
        player->SetPowerup(3);
        dead = true;
    }

    return false;
}

void PU_FeatherPowerup::nospawn(short y)
{
    state = 1;
    desty = y;
    setYi(y + TILESIZE - collisionHeight);
}
