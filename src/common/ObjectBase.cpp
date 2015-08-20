#include "ObjectBase.h"

#include "GlobalConstants.h"
#include "map.h"

extern int g_iNextNetworkID;

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
    : iw(0), ih(0)
    , velx(0.0f), vely(0.0f)
    , spr(nspr1)
    , state(0)
    , dead(false)
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

    iNetworkID = g_iNextNetworkID++;
}

/*
short CObject::writeNetworkUpdate(char * pData)
{
	//To send different messages from the same object
	//put in a message type ID that tells it to write and
	//read the message differently
	WriteIntToBuffer(&pData[0], iNetworkID);
	WriteShortToBuffer(&pData[4], ix);
	WriteShortToBuffer(&pData[6], iy);
	WriteFloatToBuffer(&pData[8], velx);
	WriteFloatToBuffer(&pData[12], vely);
	return 16;
}

void CObject::readNetworkUpdate(short size, char * pData)
{
	//ReadIntFromBuffer(&iNetworkID, &pData[0]);
	ReadShortFromBuffer(&ix, &pData[4]);
	ReadShortFromBuffer(&iy, &pData[6]);
	ReadFloatFromBuffer(&velx, &pData[8]);
	ReadFloatFromBuffer(&vely, &pData[12]);
}*/

//returns the blocks touching each of the four corners
void CObject::GetCollisionBlocks(IO_Block * blocks[4])
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

    blocks[0] = NULL;
    blocks[1] = NULL;

    if (iy >= 0 && iy < 480) {
        short yt = iy / TILESIZE;

        blocks[0] = g_map->block(xl, yt);
        blocks[1] = g_map->block(xr, yt);
    }

    blocks[2] = NULL;
    blocks[3] = NULL;

	if (iy + ih >= 0 && iy + ih < 480) {
        short yb = (iy + ih) / TILESIZE;

        blocks[2] = g_map->block(xl, yb);
        blocks[3] = g_map->block(xr, yb);
    }
}
