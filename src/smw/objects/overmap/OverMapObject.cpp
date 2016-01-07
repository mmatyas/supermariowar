#include "OverMapObject.h"

#include "gfx.h"

//------------------------------------------------------------------------------
// class OverMapObject - moving objects that don't collide with map or objects, just player
//------------------------------------------------------------------------------
IO_OverMapObject::IO_OverMapObject(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth) :
    CObject(nspr, x, y)
{
    objectType = object_overmap;
    //movingObjectType = movingobject_none;

    iNumSprites = iNumSpr;

    if (iAnimationWidth > -1)
        iw = iAnimationWidth;
    else if (spr)
        iw = (short)spr->getWidth() / iNumSprites;

    if (iAnimationHeight > -1)
        ih = iAnimationHeight;

    animationspeed = aniSpeed;
    animationtimer = 0;

    if (spr)
        animationWidth = (short)spr->getWidth();

    if (iCollisionWidth > -1) {
        collisionWidth = iCollisionWidth;
        collisionHeight = iCollisionHeight;
        collisionOffsetX = iCollisionOffsetX;
        collisionOffsetY = iCollisionOffsetY;
    } else {
        collisionWidth = iw;
        collisionHeight = ih;
        collisionOffsetX = 0;
        collisionOffsetY = 0;
    }

    if (iAnimationOffsetX > -1) {
        animationOffsetX = iAnimationOffsetX;
        animationOffsetY = iAnimationOffsetY;
    } else {
        animationOffsetX = 0;
        animationOffsetY = 0;
    }

    drawframe = animationOffsetX;
}

void IO_OverMapObject::draw()
{
    spr->draw(ix - collisionOffsetX, iy - collisionOffsetY, drawframe, animationOffsetY, iw, ih);
}

void IO_OverMapObject::draw(short iOffsetX, short iOffsetY)
{
    gfx_drawpreview(spr->getSurface(), ((ix - collisionOffsetX) >> 1) + iOffsetX, ((iy - collisionOffsetY) >> 1) + iOffsetY, drawframe >> 1, animationOffsetY >> 1, iw >> 1, ih >> 1, iOffsetX, iOffsetY, 320, 240, true);
}

void IO_OverMapObject::update()
{
    setXf(fx + velx);
    setYf(fy + vely);

    animate();
}

void IO_OverMapObject::animate()
{
    if (animationspeed > 0 && ++animationtimer == animationspeed) {
        animationtimer = 0;
        drawframe += iw;
        if (drawframe >= animationWidth)
            drawframe = animationOffsetX;
    }
}
