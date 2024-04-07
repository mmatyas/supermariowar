#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ModPowerup : public MO_Powerup
{
	public:
		PU_ModPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_ModPowerup(){};

		bool collide(CPlayer * player) override;
};
