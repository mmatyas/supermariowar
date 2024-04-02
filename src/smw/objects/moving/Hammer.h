#pragma once

#include "MovingObject.h"


class MO_Hammer : public IO_MovingObject
{
	public:
		MO_Hammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);
		~MO_Hammer(){};

		void update() override;
		bool collide(CPlayer * player) override;
		void draw() override;

		short colorOffset;

	private:
		short ttl;
		bool fSuper;
		Spotlight * sSpotlight;
};
