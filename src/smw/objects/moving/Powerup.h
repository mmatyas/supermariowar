#ifndef SMW_GAMEOBJECT_BLOCK_MO_Powerup_H
#define SMW_GAMEOBJECT_BLOCK_MO_Powerup_H

class MO_Powerup : public IO_MovingObject
{
	public:
		MO_Powerup(gfxSprite *nspr, short x, short y, short iNumSpr, short aniSpeed, short iCollisionWidth = -1, short iCollisionHeight = -1, short iCollisionOffsetX = -1, short iCollisionOffsetY = -1);
		virtual ~MO_Powerup(){};

		virtual void draw();
		virtual void update();
		virtual bool collide(CPlayer * player);
		virtual void nospawn(short y);

	protected:
		float desty;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_Powerup_H
