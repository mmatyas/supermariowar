#ifndef SMW_GAMEOBJECT_BLOCK_PU_IceWandPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_IceWandPowerup_H

class PU_IceWandPowerup : public MO_Powerup
{
	public:
		PU_IceWandPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_IceWandPowerup(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		short sparkleanimationtimer;
		short sparkledrawframe;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_IceWandPowerup_H
