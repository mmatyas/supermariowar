#ifndef SMW_GAMEOBJECT_BLOCK_PU_ExtraGuyPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_ExtraGuyPowerup_H

class PU_ExtraGuyPowerup : public MO_Powerup
{
	public:
		PU_ExtraGuyPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, bool moveToRight, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY, short type);
		~PU_ExtraGuyPowerup(){};

		bool collide(CPlayer * player);

	private:
		short iType;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_ExtraGuyPowerup_H
