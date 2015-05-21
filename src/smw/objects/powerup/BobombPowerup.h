#ifndef SMW_GAMEOBJECT_BLOCK_PU_BobombPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_BobombPowerup_H

class PU_BobombPowerup : public MO_Powerup
{
	public:
		PU_BobombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BobombPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_BobombPowerup_H
