#ifndef SMW_GAMEOBJECT_BLOCK_PU_FirePowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_FirePowerup_H

class PU_FirePowerup : public MO_Powerup
{
	public:
		PU_FirePowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FirePowerup(){};

		bool collide(CPlayer * player);

};

#endif // SMW_GAMEOBJECT_BLOCK_PU_FirePowerup_H
