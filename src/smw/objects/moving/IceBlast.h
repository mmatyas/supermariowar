#pragma once

#include "MovingObject.h"


class MO_IceBlast : public IO_MovingObject
{
	public:
		MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short iTeamID, short iColorID);
		~MO_IceBlast(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

	private:
		short ttl;
		Spotlight * sSpotlight;
};
