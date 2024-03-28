#pragma once

#include "OverMapObject.h"


class OMO_PipeBonus: public IO_OverMapObject
{
	public:
		OMO_PipeBonus(gfxSprite *nspr, float velx, float vely, short ix, short iy, short type, short duration, short uncollectabletime);
		~OMO_PipeBonus(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

    short GetType() {
        return iType;
    }

	private:
		short iType, iDuration;
		short iUncollectableTime;
};
