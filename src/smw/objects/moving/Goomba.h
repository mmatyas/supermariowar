#pragma once

#include "WalkingEnemy.h"


class MO_Goomba : public MO_WalkingEnemy
{
	public:
		MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing);
		virtual ~MO_Goomba(){};

		void draw();
		void update();
		bool hittop(CPlayer * player);
		void Die();
    void DieAndDropShell(bool fBounce, bool fFlip) {
        Die();
    }
};
