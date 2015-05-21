#ifndef SMW_GAMEOBJECT_BLOCK_PU_BulletBillPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_BulletBillPowerup_H

class PU_BulletBillPowerup : public MO_Powerup
{
	public:
		PU_BulletBillPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_BulletBillPowerup(){};

		bool collide(CPlayer * player);
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_BulletBillPowerup_H
