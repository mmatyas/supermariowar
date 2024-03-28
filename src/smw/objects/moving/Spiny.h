#pragma once

#include "WalkingEnemy.h"


class MO_Spiny : public MO_WalkingEnemy
{
	public:
		MO_Spiny(gfxSprite *nspr, bool moveToRight);
		~MO_Spiny(){};

		void update();
		bool hittop(CPlayer * player);
		void Die();
		void DropShell(bool fBounce, bool fFlip);
};
