#pragma once

#include "OverMapObject.h"


class OMO_KingOfTheHillZone : public IO_OverMapObject
{
	public:
		OMO_KingOfTheHillZone(gfxSprite *nspr);

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void placeArea();
		void reset();
    short getColorID() const {
        return colorID;
    }

	private:
		CPlayer * playersTouching[4];
		short playersTouchingCount[4];
		short totalTouchingPlayers = 0;

		short iPlayerID = -1;

		short colorID = -1;
		short scoretimer = 0;
		short frame = 0;
		short relocatetimer = 0;
		short size;

		short multiplier = 1;
		short multipliertimer = 0;

	friend class CObjectContainer;
};
