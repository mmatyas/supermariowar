#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_HammerPowerup : public MO_Powerup
{
	public:
		PU_HammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_HammerPowerup(){};

		bool collide(CPlayer * player) override;
};
