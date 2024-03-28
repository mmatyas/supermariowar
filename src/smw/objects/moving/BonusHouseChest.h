#pragma once

#include "MovingObject.h"


class MO_BonusHouseChest : public IO_MovingObject
{
	public:
		MO_BonusHouseChest(gfxSprite *nspr, short ix, short iy, short iBonusItem);
		~MO_BonusHouseChest(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		short bonusitem;
		short drawbonusitemy;
		short drawbonusitemtimer;
};
