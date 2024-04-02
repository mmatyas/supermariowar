#pragma once

#include "WalkingEnemy.h"


class MO_Spiny : public MO_WalkingEnemy
{
	public:
		MO_Spiny(gfxSprite *nspr, bool moveToRight);
		~MO_Spiny(){};

		void update() override;
		bool hittop(CPlayer * player) override;
		void Die() override;
		void DropShell(bool fBounce, bool fFlip) override;
};
