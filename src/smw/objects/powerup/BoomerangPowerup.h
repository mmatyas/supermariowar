#ifndef SMW_GAMEOBJECT_BLOCK_PU_BoomerangPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_BoomerangPowerup_H

class PU_BoomerangPowerup : public MO_Powerup
{
	public:
		PU_BoomerangPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BoomerangPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_BoomerangPowerup_H
