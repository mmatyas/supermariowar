#pragma once

#include "WalkingEnemy.h"


class MO_BuzzyBeetle : public MO_WalkingEnemy
{
	public:
		MO_BuzzyBeetle(gfxSprite *nspr, bool moveToRight);

		void update() override;
		bool hittop(CPlayer * player) override;
		void Die() override;
		void DropShell(bool fBounce, bool fFlip) override;
};
