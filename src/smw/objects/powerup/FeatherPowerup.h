#ifndef SMW_GAMEOBJECT_BLOCK_PU_FeatherPowerup_H
#define SMW_GAMEOBJECT_BLOCK_PU_FeatherPowerup_H

class PU_FeatherPowerup : public IO_MovingObject
{
	public:
		PU_FeatherPowerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth, short iCollisionHeight, short iCollisionOffsetX, short iCollisionOffsetY);
		~PU_FeatherPowerup(){};

		void update();
		void draw();
		virtual bool collide(CPlayer * player);

		void nospawn(short y);

	private:
		bool fFloatDirectionRight;
		float dFloatAngle;
		float desty;
		float dFloatCenterX, dFloatCenterY;
};

#endif // SMW_GAMEOBJECT_BLOCK_PU_FeatherPowerup_H
