#pragma once

#include "WalkingEnemy.h"


class MO_Koopa : public MO_WalkingEnemy
{
	public:
		MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing, bool fFallOffLedges);
		~MO_Koopa(){};

		void draw();
		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell(bool fBounce, bool fFlip);

	private:

		bool fRed;
};
