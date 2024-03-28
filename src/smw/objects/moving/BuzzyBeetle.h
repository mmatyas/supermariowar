#pragma once

#include "WalkingEnemy.h"


class MO_BuzzyBeetle : public MO_WalkingEnemy
{
	public:
		MO_BuzzyBeetle(gfxSprite *nspr, bool moveToRight);
		~MO_BuzzyBeetle(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell(bool fBounce, bool fFlip);
};
