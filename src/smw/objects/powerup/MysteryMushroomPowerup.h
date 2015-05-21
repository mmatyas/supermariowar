#ifndef SMW_GAMEOBJECT_BLOCK_PU_MysteryMushroomPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_MysteryMushroomPowerup_H

class PU_MysteryMushroomPowerup : public MO_Powerup
{
	public:
		PU_MysteryMushroomPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_MysteryMushroomPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_MysteryMushroomPowerup_H
