#ifndef SMW_GAMEOBJECT_BLOCK_MO_WalkingEnemy_H
#define SMW_GAMEOBJECT_BLOCK_MO_WalkingEnemy_H

class MO_WalkingEnemy : public IO_MovingObject
{
	public:
		MO_WalkingEnemy(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iAnimationOffsetX, short iAnimationOffsetY, short iAnimationHeight, short iAnimationWidth, bool moveToRight, bool killOnWeakWeapon, bool fBouncing, bool fallOffLedges);
		virtual ~MO_WalkingEnemy(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void collide(IO_MovingObject * object);
		virtual void place();

		virtual bool hittop(CPlayer * player) = 0;
		virtual bool hitother(CPlayer * player);

		virtual void ShatterDie();
		virtual void Die() {}
    virtual void DieAndDropShell(bool fBounce, bool fFlip) {
        if (frozen) {
            ShatterDie();
            return;
        }
        dead = true;
        DropShell(fBounce, fFlip);
    }
		virtual void DropShell(bool fBounce, bool fFlip) {}

    killstyle getKillStyle() {
        return killStyle;
    }

	protected:
		float spawnradius;
		float spawnangle;

		short iSpawnIconOffset;
		killstyle killStyle;

		short burnuptimer;
		bool fKillOnWeakWeapon;
		bool fBouncing;
		bool fFallOffLedges;

		bool frozen;
		short frozentimer;
		float frozenvelocity;
		short frozenanimationspeed;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_WalkingEnemy_H
