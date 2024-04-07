#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_ExtraGuyPowerup : public MO_Powerup
{
	public:
		PU_ExtraGuyPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type);
		~PU_ExtraGuyPowerup(){};

		bool collide(CPlayer * player) override;

	private:
		short iType;
};
