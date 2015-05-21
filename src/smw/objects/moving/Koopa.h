#ifndef SMW_GAMEOBJECT_BLOCK_MO_Koopa_H
#define SMW_GAMEOBJECT_BLOCK_MO_Koopa_H

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

#endif // SMW_GAMEOBJECT_BLOCK_MO_Koopa_H
