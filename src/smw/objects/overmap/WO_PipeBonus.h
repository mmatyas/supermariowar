#pragma once

#include "OverMapObject.h"


class OMO_PipeBonus: public IO_OverMapObject
{
	public:
		OMO_PipeBonus(gfxSprite *nspr, float velx, float vely, short ix, short iy, short type, short duration, short uncollectabletime);

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

    short GetType() const {
        return iType;
    }

	private:
		const short iType, iDuration;
		short iUncollectableTime;
};
