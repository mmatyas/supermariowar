#pragma once

#include "MovingObject.h"


class MO_SledgeHammer : public IO_MovingObject
{
	public:
		MO_SledgeHammer(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID, bool superHammer);

		void update() override;
		bool collide(CPlayer * player) override;
		void explode();
		void draw() override;

		short playerID;
		short teamID;
		short colorOffset;

	private:
		bool fSuper;
};
