#pragma once

#include "OverMapObject.h"


class OMO_Area : public IO_OverMapObject
{
	public:
		OMO_Area(gfxSprite *nspr, short numAreas);
		~OMO_Area(){};

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void placeArea();
		void reset();
    short getColorID() {
        return colorID;
    }
		void setOwner(CPlayer * player);

	private:
		short iPlayerID;
		short iTeamID;

		short colorID;
		short scoretimer;
		short frame;
		short relocatetimer;
		short numareas;
		short totalTouchingPlayers;
		CPlayer * touchingPlayer;

	friend class CObjectContainer;
};
