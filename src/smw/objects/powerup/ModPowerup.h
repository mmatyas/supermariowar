#ifndef SMW_GAMEOBJECT_BLOCK_PU_ModPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_ModPowerup_H

class PU_ModPowerup : public MO_Powerup
{
	public:
		PU_ModPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_ModPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_ModPowerup_H
