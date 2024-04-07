#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_BobombPowerup : public MO_Powerup
{
	public:
		PU_BobombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BobombPowerup(){};

		bool collide(CPlayer * player) override;
};
