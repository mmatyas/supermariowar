#ifndef SMW_GAMEOBJECT_BLOCK_PU_StarPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_StarPowerup_H

class PU_StarPowerup : public MO_Powerup
{
	public:
		PU_StarPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_StarPowerup(){};

		bool collide(CPlayer * player);

};

#endif // SMW_GAMEOBJECT_BLOCK_PU_StarPowerup_H
