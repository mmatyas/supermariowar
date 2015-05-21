#ifndef SMW_GAMEOBJECT_BLOCK_PU_PoisonPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_PoisonPowerup_H

class PU_PoisonPowerup : public MO_Powerup
{
	public:
		PU_PoisonPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_PoisonPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_PoisonPowerup_H
