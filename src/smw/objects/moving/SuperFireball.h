#ifndef SMW_GAMEOBJECT_BLOCK_MO_SuperFireball_H
#define SMW_GAMEOBJECT_BLOCK_MO_SuperFireball_H

class MO_SuperFireball : public IO_MovingObject
{
	public:
		MO_SuperFireball(gfxSprite *nspr, short x, short y, short iNumSpr, float fVelyX, float fVelyY, short aniSpeed, short iGlobalID, short iTeamID, short iColorID);
		~MO_SuperFireball(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

	private:
		short colorOffset;
		short directionOffset;
		short ttl;
};


#endif // SMW_GAMEOBJECT_BLOCK_MO_SuperFireball_H
