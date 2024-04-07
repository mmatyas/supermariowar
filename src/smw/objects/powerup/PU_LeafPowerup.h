#pragma once

#include "objects/powerup/PU_FeatherPowerup.h"

class CPlayer;
class gfxSprite;


class PU_LeafPowerup : public PU_FeatherPowerup
{
	public:
		PU_LeafPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_LeafPowerup(){};

		bool collide(CPlayer * player) override;
};
