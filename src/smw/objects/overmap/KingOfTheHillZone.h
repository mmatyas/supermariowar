#pragma once

#include "OverMapObject.h"


class OMO_KingOfTheHillZone : public IO_OverMapObject
{
	public:
		OMO_KingOfTheHillZone(gfxSprite *nspr);
		~OMO_KingOfTheHillZone(){};

		void draw();
		void update();
		bool collide(CPlayer * player);
		void placeArea();
		void reset();
    short getColorID() {
        return colorID;
    }

	private:
		CPlayer * playersTouching[4];
		short playersTouchingCount[4];
		short totalTouchingPlayers;

		short iPlayerID;

		short colorID;
		short scoretimer;
		short frame;
		short relocatetimer;
		short size;

		short multiplier;
		short multipliertimer;

	friend class CObjectContainer;
};
