#pragma once

#include "MovingObject.h"
#include "PlayerKillStyles.h"


class MO_Explosion : public IO_MovingObject
{
	public:
		MO_Explosion(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short id, short iTeamID, KillStyle style);
		~MO_Explosion(){};

		void update();
		bool collide(CPlayer * player);

	private:

		short timer;
		KillStyle iStyle;
};


