#ifndef SMW_GAMEOBJECT_BLOCK_PU_HammerPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_HammerPowerup_H

class PU_HammerPowerup : public MO_Powerup
{
	public:
		PU_HammerPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_HammerPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_HammerPowerup_H
