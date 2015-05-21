#ifndef SMW_GAMEOBJECT_BLOCK_MO_Goomba_H
#define SMW_GAMEOBJECT_BLOCK_MO_Goomba_H

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

#endif // SMW_GAMEOBJECT_BLOCK_MO_Goomba_H
