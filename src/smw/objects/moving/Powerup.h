#pragma once

#include "MovingObject.h"


class MO_Powerup : public IO_MovingObject
{
	public:
		MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1);
		virtual ~MO_Powerup(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void nospawn(short y);

	protected:
		float desty;
};
