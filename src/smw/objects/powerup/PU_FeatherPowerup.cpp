#include "PU_FeatherPowerup.h"

#include "Game.h"
#include "GameMode.h"
#include "GameValues.h"
#include "player.h"
#include "ResourceManager.h"

#include <cmath>

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
