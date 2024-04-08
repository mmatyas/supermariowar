#pragma once

#include "OverMapObject.h"


class OMO_PipeCoin: public IO_OverMapObject
{
	public:
		OMO_PipeCoin(gfxSprite *nspr, float velx, float vely, short ix, short iy, short teamid, short colorid, short uncollectabletime);

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

    short GetColor() const {
        return iColorID;
    }
    short GetTeam() const {
        return iTeamID;
    }

	private:
		const short iTeamID, iColorID;

		short sparkleanimationtimer = 0;
		short sparkledrawframe = 0;

		short iUncollectableTime;
};
