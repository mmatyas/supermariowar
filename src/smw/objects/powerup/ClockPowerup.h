#ifndef SMW_GAMEOBJECT_BLOCK_PU_ClockPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_ClockPowerup_H

class PU_ClockPowerup : public MO_Powerup
{
	public:
		PU_ClockPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_ClockPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_ClockPowerup_H
