#ifndef SMW_GAMEOBJECT_BLOCK_MO_Spiny_H
#define SMW_GAMEOBJECT_BLOCK_MO_Spiny_H

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

#endif // SMW_GAMEOBJECT_BLOCK_MO_Spiny_H
