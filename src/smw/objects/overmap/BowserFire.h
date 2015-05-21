#ifndef SMW_GAMEOBJECT_BLOCK_OMO_BowserFire_H
#define SMW_GAMEOBJECT_BLOCK_OMO_BowserFire_H

class OMO_BowserFire : public IO_OverMapObject
{
	public:
		OMO_BowserFire(gfxSprite *nspr, short x, short y, float velx, float vely, short id, short teamid, short colorid);
		~OMO_BowserFire(){};

		void update();
		void draw();
		bool collide(CPlayer * player);

	private:
		short iPlayerID;
		short iTeamID;
		short iColorOffsetY;
};

#endif // SMW_GAMEOBJECT_BLOCK_OMO_BowserFire_H
