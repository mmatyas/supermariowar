#pragma once

#include "WalkingEnemy.h"


class MO_Goomba : public MO_WalkingEnemy
{
	public:
		MO_Goomba(gfxSprite *nspr, bool moveToRight, bool fBouncing);

		void draw() override;
		void update() override;
		bool hittop(CPlayer * player) override;
		void Die() override;

    void DieAndDropShell(bool fBounce, bool fFlip) override {
        Die();
    }
};
