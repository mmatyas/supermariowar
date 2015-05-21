#ifndef SMW_GAMEOBJECT_BLOCK_PU_PowPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_PowPowerup_H

class PU_PowPowerup : public MO_Powerup
{
	public:
		PU_PowPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PowPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_PowPowerup_H
