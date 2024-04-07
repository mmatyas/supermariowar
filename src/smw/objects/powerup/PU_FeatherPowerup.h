#pragma once

#include "objects/moving/Powerup.h"

class CPlayer;
class gfxSprite;


class PU_FeatherPowerup : public IO_MovingObject
{
	public:
		PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FeatherPowerup(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;

		void nospawn(short y);

	private:
		bool fFloatDirectionRight;
		float dFloatAngle;
		float desty;
		float dFloatCenterX, dFloatCenterY;
};
