#pragma once

#include "MovingObject.h"


class MO_WalkingEnemy : public IO_MovingObject
{
	public:
		MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool fBouncing, bool fallOffLedges);
		virtual ~MO_WalkingEnemy(){};

		void draw() override;
		void update() override;
		bool collide(CPlayer * player) override;
		void collide(IO_MovingObject * object) override;
		virtual void place();

		virtual bool hittop(CPlayer * player) = 0;
		virtual bool hitother(CPlayer * player);

		virtual void ShatterDie();
		void Die() override {}
    virtual void DieAndDropShell(bool fBounce, bool fFlip) {
        if (frozen) {
            ShatterDie();
            return;
        }
        dead = true;
        DropShell(fBounce, fFlip);
    }
		virtual void DropShell(bool fBounce, bool fFlip) {}

    KillStyle getKillStyle() {
        return killStyle;
    }

	protected:
		float spawnradius;
		float spawnangle;

		short iSpawnIconOffset;
		KillStyle killStyle;

		short burnuptimer;
		bool fKillOnWeakWeapon;
		bool fBouncing;
		bool fFallOffLedges;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;
};
