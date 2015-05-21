#ifndef SMW_GAMEOBJECT_BLOCK_PU_BombPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_BombPowerup_H

class PU_BombPowerup : public MO_Powerup
{
	public:
		PU_BombPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BombPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_BombPowerup_H
