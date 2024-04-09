#pragma once

#include "MovingObject.h"

class Spotlight;


class MO_Hammer : public IO_MovingObject
{
	public:
		MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);

		void update() override;
		bool collide(CPlayer * player) override;
		void draw() override;

		short colorOffset;

	private:
		short ttl;
		bool fSuper;
		Spotlight * sSpotlight;
};
