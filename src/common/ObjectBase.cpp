#include "ObjectBase.h"

#include "GlobalConstants.h"
#include "map.h"

extern CMap* g_map;

float CapFallingVelocity(float vel)
{
    //if (vel < -MAXVELY)
    //    return -MAXVELY;

    if (vel > MAXVELY)
        return MAXVELY;

    return vel;
}

float CapSideVelocity(float vel)
{
    if (vel < -MAXSIDEVELY)
        return -MAXSIDEVELY;

    if (vel > MAXSIDEVELY)
        return MAXSIDEVELY;

    return vel;
}

//------------------------------------------------------------------------------
// class Object base class
//------------------------------------------------------------------------------
CObject::CObject(gfxSprite *nspr1, short x, short y)
    : spr(nspr1)
{
    setXi(x);
    setYi(y);

    if (spr) {
        iw = (short)spr->getWidth();
        ih = (short)spr->getHeight();
    }

    collisionWidth = iw;
    collisionHeight = ih;
    collisionOffsetX = 0;
    collisionOffsetY = 0;
}

std::array<IO_Block*, 4> CObject::GetCollisionBlocks() const
{
    short xl = 0;
    if (ix < 0)
        xl = (ix + 640) / TILESIZE;
    else
        xl = ix / TILESIZE;

    short xr = 0;
    if (ix + iw >= 640)
        xr = (ix + iw - 640) / TILESIZE;
    else
        xr = (ix + iw) / TILESIZE;

    std::array<IO_Block*, 4> blocks;
    blocks.fill(nullptr);

    if (iy >= 0 && iy < 480) {
        short yt = iy / TILESIZE;
        blocks[0] = g_map->block(xl, yt);
        blocks[1] = g_map->block(xr, yt);
    }

    if (iy + ih >= 0 && iy + ih < 480) {
        short yb = (iy + ih) / TILESIZE;
        blocks[2] = g_map->block(xl, yb);
        blocks[3] = g_map->block(xr, yb);
    }

    return blocks;
}
