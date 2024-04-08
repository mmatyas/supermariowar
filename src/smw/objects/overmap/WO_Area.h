#pragma once

#include "OverMapObject.h"


class OMO_Area : public IO_OverMapObject
{
	public:
		OMO_Area(gfxSprite *nspr, short numAreas);

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void placeArea();
		void reset();
    short getColorID() const {
        return colorID;
    }
		void setOwner(CPlayer * player);

	private:
		const short numareas;

		short iPlayerID = -1;
		short iTeamID = -1;

		short colorID = -1;
		short scoretimer = 0;
		short frame = 0;
		short relocatetimer= 0;
		short totalTouchingPlayers = 0;
		CPlayer * touchingPlayer= nullptr;

	friend class CObjectContainer;
};
