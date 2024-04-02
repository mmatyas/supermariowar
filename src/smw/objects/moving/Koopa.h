#pragma once

#include "WalkingEnemy.h"


class MO_Koopa : public MO_WalkingEnemy
{
	public:
		MO_Koopa(gfxSprite *nspr, bool moveToRight, bool red, bool fBouncing, bool fFallOffLedges);
		~MO_Koopa(){};

		void draw() override;
		void update() override;
		bool hittop(CPlayer * player) override;
		void Die() override;
		void DropShell(bool fBounce, bool fFlip) override;

	private:

		bool fRed;
};
