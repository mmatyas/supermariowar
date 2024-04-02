#pragma once

#include "MovingObject.h"


class MO_Podobo : public IO_MovingObject
{
	public:
		MO_Podobo(gfxSprite *nspr, short x, short y, float nspeed, short playerid, short teamid, short colorid, bool isSpawned);
		~MO_Podobo(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;
		void collide(IO_MovingObject * object) override;

	private:

		short iColorOffsetY;

		bool fIsSpawned;
		short iHiddenPlane;
};
