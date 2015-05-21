#ifndef SMW_GAMEOBJECT_BLOCK_MO_IceBlast_H
#define SMW_GAMEOBJECT_BLOCK_MO_IceBlast_H

class MO_IceBlast : public IO_MovingObject
{
	public:
		MO_IceBlast(gfxSprite *nspr, short x, short y, float fVelyX, short iGlobalID, short iTeamID, short iColorID);
		~MO_IceBlast(){};

		void update();
		bool collide(CPlayer * player);
		void draw();

	private:
		short colorOffset;
		short ttl;
		Spotlight * sSpotlight;
};

#endif // SMW_GAMEOBJECT_BLOCK_MO_IceBlast_H
