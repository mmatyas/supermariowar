#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_IceWandPowerup : public MO_Powerup
{
	public:
		PU_IceWandPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_IceWandPowerup(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
};
