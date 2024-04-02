#ifndef SMW_GAMEOBJECT_BLOCK_PU_TreasureChestBonus_H
#define SMW_GAMEOBJECT_BLOCK_PU_TreasureChestBonus_H

class PU_TreasureChestBonus : public MO_Powerup
{
	public:
		PU_TreasureChestBonus(gfxSprite *nspr, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short iBonusItem);
		~PU_TreasureChestBonus(){};

		void update() override;
		void draw() override;
		bool collide(CPlayer * player) override;
		float BottomBounce() override;

	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
		short numbounces;
		short bonusitem;
		short drawbonusitemx, drawbonusitemy;
		short drawbonusitemtimer;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_TreasureChestBonus_H
