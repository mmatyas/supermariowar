#pragma once

#include "MovingObject.h"


class MO_Fireball : public IO_MovingObject
{
	public:
		MO_Fireball(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_Fireball(){};

		void update() override;
		bool collide(CPlayer * player) override;
		void draw() override;

		short colorOffset;

	private:
		short ttl;
		Spotlight * sSpotlight;
};
