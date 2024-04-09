#include "Powerup.h"

#include "GlobalConstants.h"

//------------------------------------------------------------------------------
// class powerup
//------------------------------------------------------------------------------
MO_Powerup::MO_Powerup(gfxSprite* nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY)
    : IO_MovingObject(nspr, x, y, iNumSpr, aniSpeed, iCollisionWidth, iCollisionHeight, iCollisionOffsetX, iCollisionOffsetY)
{
    desty = fy - collisionHeight;
    movingObjectType = movingobject_powerup;
}

void MO_Powerup::draw()
{
    if (state == 0)
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, (short)(ih - fy + desty));
    else
        spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, 0, iw, ih);
}


void MO_Powerup::update()
{
    // Have the powerup grow out of the powerup block
    if (state == 0) {
        setYf(fy - 2.0f);

        if (fy <= desty) {
            fy = desty;
            state = 1;
            vely = 1.0f;
        }
    } else {  // Then have it obey the physics
        fOldX = fx;
        fOldY = fy;

        collision_detection_map();
    }

    animate();
}

bool MO_Powerup::collide(CPlayer*)
{
    if (state > 0)
        dead = true;

    return false;
}

void MO_Powerup::nospawn(short y)
{
    state = 1;
    setYi(y);
    vely = -VELJUMP / 2.0;
}
