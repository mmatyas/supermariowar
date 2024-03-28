#pragma once

#include "MovingObject.h"


class MO_FrenzyCard : public IO_MovingObject
{
	public:
		MO_FrenzyCard(gfxSprite *nspr, short iType);
		~MO_FrenzyCard(){};

		void update();
		void draw();
		bool collide(CPlayer * player);
		void placeCard();

	private:
		short timer;
		short type;

		short sparkleanimationtimer;
		short sparkledrawframe;
};
